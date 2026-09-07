#pragma once

#include <Arduino.h>
#include "DataTypes.h"

// Obstacle frame: AA 55, five big-endian uint16 values, flags, and checksum.
constexpr uint8_t OPENMV_START_HIGH = 0xAA;
constexpr uint8_t OPENMV_START_LOW = 0x55;
constexpr size_t OPENMV_PACKET_SIZE = 14;
// Open-round frame: AA 55 followed by line ID (1 = blue, 2 = orange).
constexpr size_t OPENMV_OPEN_PACKET_SIZE = 3;
constexpr uint32_t OPENMV_BAUDRATE = 115200;

class cameraReceiver{
    public:
    cameraReceiver();
    bool update(HardwareSerial &serialPort, OpenMVData &data);
    bool updateOpen(HardwareSerial &serialPort, OpenMVData &data);
    static uint16_t readUint16BigEndian(const uint8_t *bytes);

    private:




};
