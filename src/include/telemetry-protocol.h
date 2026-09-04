#pragma once

#include <Arduino.h>

namespace telemetry {

constexpr uint8_t MAGIC[4] = {'W', 'R', 'O', '1'};
constexpr uint8_t VERSION = 1;
constexpr size_t FRAME_SIZE = 64;
constexpr size_t CRC_OFFSET = FRAME_SIZE - sizeof(uint16_t);

enum Flag : uint16_t {
    OBSTACLE_SEEN = 1u << 0,
    VISION_FRESH = 1u << 1,
    SPEED_VALID = 1u << 2,
    ENCODER_VALID = 1u << 3,
    FRONT_DISTANCE_VALID = 1u << 4,
    LEFT_DISTANCE_VALID = 1u << 5,
    RIGHT_DISTANCE_VALID = 1u << 6,
    REAR_DISTANCE_VALID = 1u << 7,
    OBSTACLE_DISTANCE_VALID = 1u << 8,
    OBSTACLE_ANGLE_VALID = 1u << 9,
    STANLEY_OUTPUT_VALID = 1u << 10,
    TANGENTIAL_OUTPUT_VALID = 1u << 11,
    STEERING_COMMAND_VALID = 1u << 12,
    PREVIOUS_TEENSY_UART_FRAME_DROPPED = 1u << 13,
};

// Wire format v1. All multi-byte fields are little-endian. Both the Teensy 4.0
// and ESP32-C6 are little-endian, but receivers still validate the raw bytes
// instead of relying on alignment or compiler-specific struct padding.
struct __attribute__((packed)) FrameV1 {
    uint8_t magic[4];
    uint8_t version;
    uint8_t frameSize;
    uint16_t flags;
    uint32_t sequence;
    uint32_t teensyTimeUs;
    float speedMmPerSecond;
    uint8_t lap;
    uint8_t task;
    int8_t direction;
    uint8_t controllerMode;
    uint16_t frontDistanceMm;
    uint16_t leftDistanceMm;
    uint16_t rightDistanceMm;
    uint16_t rearDistanceMm;
    float obstacleDistanceMm;
    float obstacleAngleDeg;
    int32_t encoderTicks;
    float encoderDistanceMm;
    float stanleyOutputDeg;
    float tangentialOutputDeg;
    float steeringCommandDeg;
    int16_t motorPwm;
    uint16_t crc16;
};

static_assert(sizeof(FrameV1) == FRAME_SIZE,
              "Telemetry frame layout must remain exactly 64 bytes");

inline uint16_t readUint16Le(const uint8_t* bytes) {
    return static_cast<uint16_t>(bytes[0]) |
           (static_cast<uint16_t>(bytes[1]) << 8);
}

// CRC-16/CCITT-FALSE: polynomial 0x1021, initial value 0xFFFF.
inline uint16_t crc16CcittFalse(const uint8_t* data, size_t length) {
    uint16_t crc = 0xFFFF;
    for (size_t index = 0; index < length; ++index) {
        crc ^= static_cast<uint16_t>(data[index]) << 8;
        for (uint8_t bit = 0; bit < 8; ++bit) {
            crc = (crc & 0x8000u) != 0
                ? static_cast<uint16_t>((crc << 1) ^ 0x1021u)
                : static_cast<uint16_t>(crc << 1);
        }
    }
    return crc;
}

inline bool hasValidEnvelope(const uint8_t* frame, size_t length) {
    if (frame == nullptr || length != FRAME_SIZE) {
        return false;
    }
    return frame[0] == MAGIC[0] && frame[1] == MAGIC[1] &&
           frame[2] == MAGIC[2] && frame[3] == MAGIC[3] &&
           frame[4] == VERSION && frame[5] == FRAME_SIZE;
}

inline bool isValidFrame(const uint8_t* frame, size_t length) {
    if (!hasValidEnvelope(frame, length)) {
        return false;
    }
    const uint16_t receivedCrc = readUint16Le(frame + CRC_OFFSET);
    return receivedCrc == crc16CcittFalse(frame, CRC_OFFSET);
}

}  // namespace telemetry
