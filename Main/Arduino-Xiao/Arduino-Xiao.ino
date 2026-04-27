#include "TOF4Walls.h"
#include "Wire.h"
#include "DebugLog.h"

#define SERIAL_PC_SPEED 115200
#define SERIAL_TEENSY_SPEED 2000000

constexpr int LPN_FRONT = 2;
constexpr int LPN_BACK  = 3;
constexpr int LPN_LEFT  = 0;
constexpr int LPN_RIGHT = 1;




TOF4Walls tofs(Wire, LPN_FRONT, LPN_BACK, LPN_LEFT, LPN_RIGHT);

void setup() {
    pinMode(13,OUTPUT);
    digitalWrite(13, LOW);
    Serial.begin(SERIAL_PC_SPEED);
    Serial1.begin(SERIAL_TEENSY_SPEED, SERIAL_8N1, D7, D6);
    delay(1000);
    DEBUG_LOGL("Initing...");
    if (!tofs.begin(30)) {
        DEBUG_LOGL("Error, pls reboot");
        while (true) {  }
    }

    DEBUG_LOGL("All TOFs ready");
}

void loop() {
    tofs.update();
    if (tofs.hasFreshData(TOF4Walls::FRONT)) {
        uint16_t front = (uint16_t)tofs.getDistance(TOF4Walls::FRONT);
        uint16_t left  = (uint16_t)tofs.getDistance(TOF4Walls::LEFT);
        uint16_t right = (uint16_t)tofs.getDistance(TOF4Walls::BACK);

        DEBUG_LOG("F: ");
        DEBUG_LOG(front);
        DEBUG_LOG("  L: ");
        DEBUG_LOG(left);
        DEBUG_LOG("  R: ");
        DEBUG_LOGL(right);

        uint8_t lowByteFront = lowByte(front);
        uint8_t highByteFront = highByte(front);

        uint8_t lowByteLeft = lowByte(left);
        uint8_t highByteLeft = highByte(left);

        uint8_t lowByteRight = lowByte(right);
        uint8_t highByteRight = highByte(right);

        uint8_t checksum = lowByteFront + highByteFront + lowByteLeft + highByteLeft + lowByteRight + highByteRight;

        Serial1.write(0xAA);// start byte

        Serial1.write(lowByteFront);
        Serial1.write(highByteFront);

        Serial1.write(lowByteLeft);
        Serial1.write(highByteLeft);

        Serial1.write(lowByteRight);
        Serial1.write(highByteRight);
        Serial1.write(checksum);
    }
}