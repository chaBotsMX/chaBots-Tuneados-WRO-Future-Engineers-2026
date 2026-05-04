#include "DebugComms.h"

DebugComms::DebugComms(bool _wifiMode, uint8_t _txRate, const char* _existingSSID, const char* _existingPass) {
    wifiMode = _wifiMode;
    TX_RATE_HZ = _txRate;

    TRANSMITTING_RATE = 1000 / TX_RATE_HZ;
    // if set to connect to an existing wifi network, use the provided credentials.
    if (wifiMode == 0) {
        EXISTING_WIFI_SSID = _existingSSID;
        EXISTING_WIFI_PASS = _existingPass;
    }
}
void DebugComms::begin() {
    // if wifiMode is set to 0, connect to an existing wifi network using the provided credentials.
    if (wifiMode == 0) {
        WiFi.mode(WIFI_STA);
        WiFi.begin(EXISTING_WIFI_SSID, EXISTING_WIFI_PASS);
        DEBUG_LOGL("Connecting to existing WiFi...");
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            DEBUG_LOG(".");
        }
        DEBUG_LOGL("Connected to WiFi!");
    } else { // else it creates a wifi network using the provided credentials for the AP.
        WiFi.mode(WIFI_AP);
        WiFi.softAP(AP_WIFI_SSID, AP_WIFI_PASS);
        DEBUG_LOGL("AP Mode: Created WiFi network");
    }
    udp.begin(UDP_PORT);
}

void DebugComms::setIp() {
    // set the ip once the wifi is connected. if in AP mode, set it to the expected laptop ip, else set it to the broadcast ip.
    if (wifiMode == 0) {
        sendingIP = getBroadcastIP();
    } else {
        sendingIP = laptopIP;
    }
}
void DebugComms::sendDebugData(const uint8_t data[], size_t length) {
    // only send data if the transmission timer has elapsed, this avoid overloading the network with too many packets.
    if (debugCommsTimer > TRANSMITTING_RATE) {
        debugCommsTimer = 0;
        udp.beginPacket(sendingIP, UDP_PORT);
        udp.write(data, length);
        udp.endPacket();
    }
}