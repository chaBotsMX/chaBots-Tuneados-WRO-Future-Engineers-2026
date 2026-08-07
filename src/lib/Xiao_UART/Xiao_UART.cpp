/**
 * @file Xiao_UART.cpp
 * @brief UART interface for communication with the Xiao microcontroller
 *
 * @author Roy Barron / chaBotsMX
 * @date 06/07/26
 */

#include "Xiao_UART.h"

Xiao_UART::Xiao_UART(HardwareSerial& serialXiao,uint32_t baudRate) : _serialXiao(serialXiao)
{
    _serialXiao.begin(baudRate);
}

void Xiao_UART::readData()
{
    // Read all available bytes from the serial buffer, using while instead of if to ensure we process all incoming data in one go, avoiding overflow issues.
    while (_serialXiao.available() > 0) {
        int rawByte = _serialXiao.read();

        // If read() returns -1, it indicates no more data is available, so we break the loop.
        if (rawByte < 0) {
            break;
        }

        // Cast the raw byte to uint8_t for processing
        uint8_t incomingByte = static_cast<uint8_t>(rawByte);
        // check for start byte sequence (0xAA, 0x55) to begin receiving a packet and resincronice if neccesary.
        if (possibleStartByte) {
            if (incomingByte == 0x55) {
                receivingPacket = true;
                payloadIndex = 0;
                calculatedChecksum = 0;
                possibleStartByte = false;

                continue;
            }
            if (receivingPacket) {
                processPacketByte(0xAA);
            }

            possibleStartByte = false;
        }
        if (incomingByte == 0xAA) {
            possibleStartByte = true;
            continue;
        }
        if (!receivingPacket) {
            continue;
        }

        // if is not a start byte just continue processing the packet.
        processPacketByte(incomingByte);
    }
}

void Xiao_UART::processPacketByte(uint8_t incomingByte)
{

// If we haven't received the full payload yet, store the incoming byte and update the checksum.
    if (payloadIndex < PAYLOAD_SIZE) {
        payload[payloadIndex] = incomingByte;
        calculatedChecksum += incomingByte;
        payloadIndex++;

        return;
    }
    //received checksum byte, validate the packet and update the SensorData structure if valid.
    receivedChecksum = incomingByte;
    receivingPacket = false;
    payloadIndex = 0;

    if (receivedChecksum != calculatedChecksum) {
        return;
    }
    data.front =
        static_cast<uint16_t>(payload[0]) |
        (static_cast<uint16_t>(payload[1]) << 8);

    data.left =
        static_cast<uint16_t>(payload[2]) |
        (static_cast<uint16_t>(payload[3]) << 8);

    data.right =
        static_cast<uint16_t>(payload[4]) |
        (static_cast<uint16_t>(payload[5]) << 8);

    data.back =
        static_cast<uint16_t>(payload[6]) |
        (static_cast<uint16_t>(payload[7]) << 8);
    // Mark that new data is available for retrieval.
    newDataAvailable = true;
}