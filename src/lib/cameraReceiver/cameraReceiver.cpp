#include "cameraReceiver.h"

cameraReceiver::cameraReceiver() = default;

uint16_t cameraReceiver::readUint16BigEndian(const uint8_t *bytes) {
    return (static_cast<uint16_t>(bytes[0]) << 8) |
           static_cast<uint16_t>(bytes[1]);
}

bool cameraReceiver::update(HardwareSerial &serialPort, OpenMVData &data) {
   static uint8_t packet[OPENMV_PACKET_SIZE];
    static size_t packetIndex = 0;
    bool receivedValidPacket = false;

    while (serialPort.available() > 0) {
        const uint8_t incomingByte =
            static_cast<uint8_t>(serialPort.read());

        if (packetIndex == 0) {
            if (incomingByte == OPENMV_START_HIGH) {
                packet[0] = incomingByte;
                packetIndex = 1;
            }
            continue;
        }

        if (packetIndex == 1) {
            if (incomingByte == OPENMV_START_LOW) {
                packet[1] = incomingByte;
                packetIndex = 2;
            } else if (incomingByte == OPENMV_START_HIGH) {
                packet[0] = incomingByte;
                packetIndex = 1;
            } else {
                packetIndex = 0;
            }
            continue;
        }

        packet[packetIndex++] = incomingByte;

        if (packetIndex < OPENMV_PACKET_SIZE) {
            continue;
        }

        packetIndex = 0;

        uint8_t calculatedChecksum = 0;
        for (size_t index = 2; index <= 12; ++index) {
            calculatedChecksum ^= packet[index];
        }

        if (calculatedChecksum != packet[13]) {
            continue;
        }

        data.obstacleX = readUint16BigEndian(&packet[2]);
        data.obstacleY = readUint16BigEndian(&packet[4]);
        data.wallX = readUint16BigEndian(&packet[6]);
        data.wallY = readUint16BigEndian(&packet[8]);
        data.blueLineBottomY = readUint16BigEndian(&packet[10]);

        data.flags = packet[12];
        data.obstacleDetected = (data.flags & 0x01) != 0;
        data.wallDetected = (data.flags & 0x02) != 0;
        data.blueLineDetected = data.blueLineBottomY != 250;
        data.obstacleColor = (data.flags >> 2) & 0x03;
        data.receivedAtMs = millis();

        receivedValidPacket = true;
    }

    return receivedValidPacket;
}

bool cameraReceiver::updateOpen(HardwareSerial &serialPort, OpenMVData &data){
    static uint8_t packet[OPENMV_OPEN_PACKET_SIZE];
    static size_t packetIndex = 0;
    bool receivedValidPacket = false;

    while (serialPort.available() > 0) {
        const uint8_t incomingByte =
            static_cast<uint8_t>(serialPort.read());

        if (packetIndex == 0) {
            if (incomingByte == OPENMV_START_HIGH) {
                packet[0] = incomingByte;
                packetIndex = 1;
            }
            continue;
        }

        if (packetIndex == 1) {
            if (incomingByte == OPENMV_START_LOW) {
                packet[1] = incomingByte;
                packetIndex = 2;
            } else if (incomingByte == OPENMV_START_HIGH) {
                packet[0] = incomingByte;
                packetIndex = 1;
            } else {
                packetIndex = 0;
            }
            continue;
        }

        packet[2] = incomingByte;
        packetIndex = 0;

        if (packet[2] != 1 && packet[2] != 2 && packet[2] != 3) {
            continue;
        }

        data.lineId = packet[2];
        data.lineDetected = true;
        data.blueLineDetected = data.lineId == 1;
        data.receivedAtMs = millis();
        receivedValidPacket = true;
    }

    return receivedValidPacket;
}
