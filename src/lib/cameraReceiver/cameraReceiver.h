#pragma once

#include <Arduino.h>
#include "DataTypes.h"

// OpenMV frame: AA 55, five big-endian uint16 values, flags, and XOR checksum.
constexpr uint8_t OPENMV_START_HIGH = 0xAA;
constexpr uint8_t OPENMV_START_LOW = 0x55;
constexpr size_t OPENMV_PACKET_SIZE = 14;
constexpr uint32_t OPENMV_BAUDRATE = 115200;

class cameraReceiver{
    public:
    cameraReceiver();
    // Consumes all available bytes and updates data only from a complete frame.
    bool update(HardwareSerial &serialPort, OpenMVData &data);
    static uint16_t readUint16BigEndian(const uint8_t *bytes);

    private:




};
