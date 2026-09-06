#include <Arduino.h>
#include <WiFi.h>

#include <atomic>
#include <errno.h>
#include <fcntl.h>
#include <lwip/sockets.h>

#include "telemetry-protocol.h"

namespace {

constexpr uint32_t USB_SERIAL_BAUD = 115200;
constexpr uint32_t TEENSY_UART_BAUD = 2000000;
constexpr int TEENSY_UART_RX_PIN = D7;
constexpr size_t UART_RX_BUFFER_SIZE = 8192;
constexpr size_t UART_READ_CHUNK_SIZE = 1024;

constexpr char WIFI_SSID[] = "WRO-Debug-C6";
constexpr char WIFI_PASSWORD[] = "WROdebug26";
constexpr uint8_t WIFI_CHANNEL = 6;
constexpr uint16_t TELEMETRY_UDP_PORT = 3333;

constexpr size_t MAX_SUBSCRIBERS = 1;
constexpr uint32_t SUBSCRIBER_TIMEOUT_MS = 3000;
constexpr size_t MAX_CONTROL_PACKETS_PER_LOOP = 2;
constexpr uint32_t MIN_PUBLISH_INTERVAL_US = 5000;  // At most 200 Hz.
constexpr uint32_t NETWORK_RETRY_INTERVAL_MS = 2000;
constexpr uint32_t NETWORK_HEALTH_INTERVAL_MS = 1000;

constexpr uint8_t SUBSCRIBE_MESSAGE[] = {'S', 'U', 'B', '1'};
constexpr uint8_t UNSUBSCRIBE_MESSAGE[] = {'U', 'N', 'S', '1'};
struct BridgeStats {
    uint32_t validUartFrames = 0;
    uint32_t crcErrors = 0;
    uint32_t envelopeErrors = 0;
    uint32_t coalescedFrames = 0;
    uint32_t udpSendErrors = 0;
    uint32_t udpReceiveErrors = 0;
};

BridgeStats stats;
std::atomic<uint32_t> uartHardwareErrors{0};

struct __attribute__((packed)) BridgeStatusV1 {
    uint8_t magic[4];
    uint8_t version;
    uint8_t telemetryFrameSize;
    uint16_t statusSize;
    uint32_t validUartFrames;
    uint32_t crcErrors;
    uint32_t envelopeErrors;
    uint32_t coalescedFrames;
    uint32_t udpSendErrors;
    uint32_t udpReceiveErrors;
    uint32_t uartHardwareErrors;
};

static_assert(sizeof(BridgeStatusV1) == 36,
              "Bridge status packet layout must remain exactly 36 bytes");

class UartFrameParser {
public:
    bool push(uint8_t byte) {
        if (index_ < sizeof(telemetry::MAGIC)) {
            consumeMagicByte(byte);
            return false;
        }

        frame_[index_++] = byte;

        if (index_ == 6 &&
            (frame_[4] != telemetry::VERSION ||
             frame_[5] != telemetry::FRAME_SIZE)) {
            ++stats.envelopeErrors;
            resynchronize(index_);
            return false;
        }

        if (index_ < telemetry::FRAME_SIZE) {
            return false;
        }

        if (!telemetry::isValidFrame(frame_, telemetry::FRAME_SIZE)) {
            ++stats.crcErrors;
            resynchronize(telemetry::FRAME_SIZE);
            return false;
        }

        index_ = 0;
        return true;
    }

    const uint8_t* frame() const {
        return frame_;
    }

private:
    uint8_t frame_[telemetry::FRAME_SIZE] = {};
    size_t index_ = 0;

    void consumeMagicByte(uint8_t byte) {
        if (byte == telemetry::MAGIC[index_]) {
            frame_[index_++] = byte;
            return;
        }

        index_ = byte == telemetry::MAGIC[0] ? 1 : 0;
        if (index_ == 1) {
            frame_[0] = byte;
        }
    }

    void resynchronize(size_t bufferedLength) {
        // A rejected candidate can already contain the following frame if a
        // byte was lost. Preserve the newest full magic marker, or otherwise
        // the longest partial marker at the end of the buffer.
        size_t newestMagic = bufferedLength;
        for (size_t start = 1;
             start + sizeof(telemetry::MAGIC) <= bufferedLength; ++start) {
            if (memcmp(frame_ + start, telemetry::MAGIC,
                       sizeof(telemetry::MAGIC)) == 0) {
                newestMagic = start;
            }
        }

        if (newestMagic < bufferedLength) {
            index_ = bufferedLength - newestMagic;
            memmove(frame_, frame_ + newestMagic, index_);
            return;
        }

        const size_t maxPartial = min(
            sizeof(telemetry::MAGIC) - 1,
            bufferedLength
        );
        for (size_t length = maxPartial; length > 0; --length) {
            if (memcmp(frame_ + bufferedLength - length,
                       telemetry::MAGIC, length) == 0) {
                memmove(frame_, frame_ + bufferedLength - length, length);
                index_ = length;
                return;
            }
        }

        index_ = 0;
    }
};

struct Subscriber {
    sockaddr_in endpoint = {};
    uint32_t lastSeenMs = 0;
    bool active = false;
};

UartFrameParser uartParser;
Subscriber subscribers[MAX_SUBSCRIBERS];
int telemetrySocket = -1;
bool accessPointReady = false;
bool networkReady = false;
uint32_t lastNetworkAttemptMs = 0;
uint32_t lastNetworkHealthMs = 0;
uint32_t lastPublishUs = 0;
uint8_t latestFrame[telemetry::FRAME_SIZE] = {};
bool latestFrameAvailable = false;
bool framePending = false;
uint8_t uartReadChunk[UART_READ_CHUNK_SIZE] = {};

void closeTelemetrySocket();

bool socketErrorIsTransient(int errorCode) {
    return errorCode == EAGAIN || errorCode == EWOULDBLOCK ||
           errorCode == EINTR || errorCode == ENOBUFS ||
           errorCode == ENOMEM;
}

void markNetworkUnavailable() {
    closeTelemetrySocket();
    networkReady = false;
    lastNetworkAttemptMs = millis();
}

bool endpointMatches(const Subscriber& subscriber,
                     const sockaddr_in& endpoint) {
    return subscriber.active &&
           subscriber.endpoint.sin_addr.s_addr == endpoint.sin_addr.s_addr &&
           subscriber.endpoint.sin_port == endpoint.sin_port;
}

bool sameAddress(const Subscriber& subscriber,
                 const sockaddr_in& endpoint) {
    return subscriber.active &&
           subscriber.endpoint.sin_addr.s_addr == endpoint.sin_addr.s_addr;
}

bool subscriberIsFresh(const Subscriber& subscriber, uint32_t nowMs) {
    return subscriber.active &&
           static_cast<uint32_t>(nowMs - subscriber.lastSeenMs) <=
               SUBSCRIBER_TIMEOUT_MS;
}

bool sendDatagram(const uint8_t* data, size_t length,
                  const sockaddr_in& endpoint) {
    if (telemetrySocket < 0) {
        return false;
    }

    const ssize_t sent = sendto(
        telemetrySocket, data, length, MSG_DONTWAIT,
        reinterpret_cast<const sockaddr*>(&endpoint), sizeof(endpoint)
    );
    if (sent != static_cast<ssize_t>(length)) {
        ++stats.udpSendErrors;
        if (sent < 0) {
            const int errorCode = errno;
            if (!socketErrorIsTransient(errorCode)) {
                markNetworkUnavailable();
            }
        }
        return false;
    }
    return true;
}

void sendAck(const sockaddr_in& endpoint) {
    const BridgeStatusV1 status = {
        {'A', 'C', 'K', '1'},
        telemetry::VERSION,
        static_cast<uint8_t>(telemetry::FRAME_SIZE),
        sizeof(BridgeStatusV1),
        stats.validUartFrames,
        stats.crcErrors,
        stats.envelopeErrors,
        stats.coalescedFrames,
        stats.udpSendErrors,
        stats.udpReceiveErrors,
        uartHardwareErrors.load(std::memory_order_relaxed),
    };
    sendDatagram(reinterpret_cast<const uint8_t*>(&status), sizeof(status),
                 endpoint);
}

void removeSubscriber(const sockaddr_in& endpoint) {
    for (Subscriber& subscriber : subscribers) {
        if (endpointMatches(subscriber, endpoint)) {
            subscriber.active = false;
        }
    }
}

void addOrRefreshSubscriber(const sockaddr_in& endpoint) {
    const uint32_t now = millis();

    for (Subscriber& subscriber : subscribers) {
        // The AP accepts a single station. Allow that laptop to restart the app
        // with a new ephemeral UDP port without waiting for the old port to age.
        if (sameAddress(subscriber, endpoint)) {
            subscriber.endpoint = endpoint;
            subscriber.lastSeenMs = now;
            framePending = latestFrameAvailable;
            sendAck(endpoint);
            return;
        }
    }

    for (Subscriber& subscriber : subscribers) {
        if (!subscriberIsFresh(subscriber, now)) {
            subscriber.endpoint = endpoint;
            subscriber.lastSeenMs = now;
            subscriber.active = true;
            framePending = latestFrameAvailable;
            sendAck(endpoint);
            return;
        }
    }

    // A still-active endpoint is never evicted by another address.
}

void serviceControlPackets() {
    if (!networkReady || telemetrySocket < 0) {
        return;
    }

    for (size_t handled = 0; handled < MAX_CONTROL_PACKETS_PER_LOOP;
         ++handled) {
        sockaddr_in remoteEndpoint = {};
        socklen_t endpointLength = sizeof(remoteEndpoint);
        uint8_t message[8] = {};
        const ssize_t bytesRead = recvfrom(
            telemetrySocket, message, sizeof(message), MSG_DONTWAIT,
            reinterpret_cast<sockaddr*>(&remoteEndpoint), &endpointLength
        );

        if (bytesRead < 0) {
            const int errorCode = errno;
            if (!socketErrorIsTransient(errorCode)) {
                ++stats.udpReceiveErrors;
                markNetworkUnavailable();
            }
            return;
        }
        if (remoteEndpoint.sin_family != AF_INET) {
            continue;
        }

        if (bytesRead == static_cast<ssize_t>(sizeof(SUBSCRIBE_MESSAGE)) &&
            memcmp(message, SUBSCRIBE_MESSAGE,
                   sizeof(SUBSCRIBE_MESSAGE)) == 0) {
            addOrRefreshSubscriber(remoteEndpoint);
        } else if (
            bytesRead == static_cast<ssize_t>(sizeof(UNSUBSCRIBE_MESSAGE)) &&
            memcmp(message, UNSUBSCRIBE_MESSAGE,
                   sizeof(UNSUBSCRIBE_MESSAGE)) == 0) {
            removeSubscriber(remoteEndpoint);
        }
    }
}

void queueLatestFrame(const uint8_t* frame) {
    if (framePending) {
        ++stats.coalescedFrames;
    }
    memcpy(latestFrame, frame, telemetry::FRAME_SIZE);
    latestFrameAvailable = true;
    framePending = true;
}

void serviceTeensyUart() {
    const int availableBytes = Serial1.available();
    if (availableBytes <= 0) {
        return;
    }

    const size_t requested = min(
        static_cast<size_t>(availableBytes), sizeof(uartReadChunk)
    );
    const size_t bytesRead = Serial1.read(uartReadChunk, requested);
    for (size_t index = 0; index < bytesRead; ++index) {
        if (uartParser.push(uartReadChunk[index])) {
            ++stats.validUartFrames;
            queueLatestFrame(uartParser.frame());
        }
    }
}

void publishPendingFrame() {
    if (!networkReady || !framePending) {
        return;
    }

    const uint32_t nowMs = millis();
    bool hasSubscriber = false;
    for (Subscriber& subscriber : subscribers) {
        if (!subscriberIsFresh(subscriber, nowMs)) {
            subscriber.active = false;
            continue;
        }
        hasSubscriber = true;
    }

    if (!hasSubscriber) {
        framePending = false;
        return;
    }

    const uint32_t nowUs = micros();
    if (static_cast<uint32_t>(nowUs - lastPublishUs) <
        MIN_PUBLISH_INTERVAL_US) {
        return;
    }

    // Mark it consumed before touching the network. A failed or busy socket
    // drops this sample; telemetry is never retried or allowed to form a queue.
    framePending = false;
    lastPublishUs = nowUs;
    for (const Subscriber& subscriber : subscribers) {
        if (subscriber.active) {
            sendDatagram(latestFrame, telemetry::FRAME_SIZE,
                         subscriber.endpoint);
        }
    }
}

void closeTelemetrySocket() {
    if (telemetrySocket >= 0) {
        close(telemetrySocket);
        telemetrySocket = -1;
    }
}

bool startTelemetrySocket() {
    closeTelemetrySocket();

    const int udpSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (udpSocket < 0) {
        return false;
    }

    const int reuseAddress = 1;
    if (setsockopt(udpSocket, SOL_SOCKET, SO_REUSEADDR, &reuseAddress,
                   sizeof(reuseAddress)) < 0) {
        close(udpSocket);
        return false;
    }

    sockaddr_in localEndpoint = {};
    localEndpoint.sin_family = AF_INET;
    localEndpoint.sin_port = htons(TELEMETRY_UDP_PORT);
    localEndpoint.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(udpSocket, reinterpret_cast<sockaddr*>(&localEndpoint),
             sizeof(localEndpoint)) < 0) {
        close(udpSocket);
        return false;
    }

    const int currentFlags = fcntl(udpSocket, F_GETFL, 0);
    if (currentFlags < 0 ||
        fcntl(udpSocket, F_SETFL, currentFlags | O_NONBLOCK) < 0) {
        close(udpSocket);
        return false;
    }

    telemetrySocket = udpSocket;
    return true;
}

bool configureAccessPoint() {
    WiFi.softAPdisconnect(true);

    if (!WiFi.mode(WIFI_AP)) {
        return false;
    }
    if (!WiFi.setSleep(false)) {
        return false;
    }

    const IPAddress address(192, 168, 4, 1);
    const IPAddress gateway(192, 168, 4, 1);
    const IPAddress subnet(255, 255, 255, 0);
    if (!WiFi.softAPConfig(address, gateway, subnet)) {
        return false;
    }
    if (!WiFi.softAP(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL, false,
                     MAX_SUBSCRIBERS)) {
        return false;
    }
    return true;
}

void attemptNetworkStartup() {
    if (!accessPointReady) {
        accessPointReady = configureAccessPoint();
    }
    networkReady = accessPointReady && startTelemetrySocket();
    // Measure retry spacing from the end of the potentially slow operation.
    lastNetworkAttemptMs = millis();

    if (networkReady) {
        Serial.println("WRO telemetry bridge ready");
        Serial.print("Wi-Fi: ");
        Serial.println(WIFI_SSID);
        Serial.print("UDP: ");
        Serial.print(WiFi.softAPIP());
        Serial.print(':');
        Serial.println(TELEMETRY_UDP_PORT);
    } else {
        Serial.println("Telemetry network startup failed; retrying");
    }
}

void serviceNetworkHealth() {
    if (!networkReady ||
        static_cast<uint32_t>(millis() - lastNetworkHealthMs) <
            NETWORK_HEALTH_INTERVAL_MS) {
        return;
    }
    lastNetworkHealthMs = millis();

    const wifi_mode_t mode = WiFi.getMode();
    if ((static_cast<uint8_t>(mode) & static_cast<uint8_t>(WIFI_AP)) == 0) {
        accessPointReady = false;
        markNetworkUnavailable();
    }
}

void serviceNetworkStartup() {
    if (networkReady) {
        return;
    }
    if (static_cast<uint32_t>(millis() - lastNetworkAttemptMs) >=
        NETWORK_RETRY_INTERVAL_MS) {
        attemptNetworkStartup();
    }
}

void handleUartError(hardwareSerial_error_t) {
    uartHardwareErrors.fetch_add(1, std::memory_order_relaxed);
}

}  // namespace

void setup() {
    Serial.begin(USB_SERIAL_BAUD);

    // The UART receive path has no timeout calls and never invokes Wi-Fi.
    Serial1.setRxBufferSize(UART_RX_BUFFER_SIZE);
    Serial1.begin(TEENSY_UART_BAUD, SERIAL_8N1, TEENSY_UART_RX_PIN, -1);
    Serial1.onReceiveError(handleUartError);

    WiFi.persistent(false);
    attemptNetworkStartup();

    if (!Serial1) {
        Serial.println("Error: Teensy UART could not be initialized");
    }
}

void loop() {
    // Ingestion, control traffic, and publication are deliberately decoupled.
    // Every stage is bounded; stale telemetry is replaced instead of queued.
    serviceTeensyUart();
    serviceNetworkHealth();
    serviceNetworkStartup();
    serviceControlPackets();
    publishPendingFrame();
}
