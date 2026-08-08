#ifndef XIAO_UART_H
#define XIAO_UART_H

#include <Arduino.h>

struct SensorData {
    uint16_t front = 0;
    uint16_t left  = 0;
    uint16_t right = 0;
    uint16_t back  = 0;
};

class Xiao_UART {
public:
    Xiao_UART(HardwareSerial& serialXiao, uint32_t baudRate);

    void readData();

    bool available() const {
        return newDataAvailable;
    }

    SensorData getData() {
        newDataAvailable = false;
        return data;
    }

private:
    static constexpr uint8_t PAYLOAD_SIZE = 8;

    HardwareSerial& _serialXiao;

    SensorData data;

    uint8_t payload[PAYLOAD_SIZE] = {};
    uint8_t payloadIndex = 0;

    uint8_t calculatedChecksum = 0;
    uint8_t receivedChecksum = 0;

    bool receivingPacket = false;
    bool possibleStartByte = false;
    bool newDataAvailable = false;

    void processPacketByte(uint8_t incomingByte);
};

#endif // XIAO_UART_H