#pragma once

#include <Arduino.h>
#include "DataTypes.h"

constexpr uint8_t OPENMV_START_HIGH = 0xAA;
constexpr uint8_t OPENMV_START_LOW = 0x55;
constexpr size_t OPENMV_PACKET_SIZE = 14;
constexpr uint32_t OPENMV_BAUDRATE = 115200;

class cameraReceiver{
    public:
    cameraReceiver();
    bool update(HardwareSerial &serialPort, OpenMVData &data);
    static uint16_t readUint16BigEndian(const uint8_t *bytes);

    private:




};
