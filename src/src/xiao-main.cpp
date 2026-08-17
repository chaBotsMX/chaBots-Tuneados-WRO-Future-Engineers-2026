#include "TOF4Walls.h"
#include "Wire.h"
#include "DebugLog.h"

#define LED_PIN 13

#define SERIAL_PC_SPEED 115200
#define SERIAL_TEENSY_SPEED 2000000

#define RX_PIN D7
#define TX_PIN D6

#define INITIALIZE_DELAY 1000
constexpr int LPN_FRONT = D2;
constexpr int LPN_RIGHT  = D1;
constexpr int LPN_LEFT  = D0;
constexpr int LPN_BACK = D3;




TOF4Walls tofs(Wire, LPN_FRONT, LPN_RIGHT, LPN_LEFT, LPN_BACK);

void setup() {
    //pinMode(LED_PIN,OUTPUT);
    digitalWrite(LED_PIN, LOW);
    //initialize serial ports for communication with PC and Teensy
    Serial.begin(SERIAL_PC_SPEED);
    Serial1.begin(SERIAL_TEENSY_SPEED, SERIAL_8N1, RX_PIN, TX_PIN);

    //initial delay to allow the xiao to boot up and be ready for communication
    delay(INITIALIZE_DELAY);
    DEBUG_LOGL("Initing...");

    //initialize the TOF sensors, if any of them fails to initialize, the program will halt and print an error message.
    if (!tofs.begin(60)) {
        DEBUG_LOGL("Error, pls reboot");
        while (true) {  }
    }

    DEBUG_LOGL("All TOFs ready");
}

void loop() {
    tofs.update();
    if (tofs.hasFreshData(TOF4Walls::FRONT) || tofs.hasFreshData(TOF4Walls::LEFT) || tofs.hasFreshData(TOF4Walls::RIGHT) ) {
        uint16_t front = (uint16_t)tofs.getDistance(TOF4Walls::FRONT);
        uint16_t left  = (uint16_t)tofs.getDistance(TOF4Walls::LEFT);
        uint16_t right = (uint16_t)tofs.getDistance(TOF4Walls::RIGHT);

        DEBUG_LOG("F: ");
        DEBUG_LOG(front);
        DEBUG_LOG("  L: ");
        DEBUG_LOG(left);
        DEBUG_LOG("  R: ");
        DEBUG_LOGL(right);

        //digitalWrite(LED_PIN, HIGH);
        uint8_t lowByteFront = lowByte(front);
        uint8_t highByteFront = highByte(front);

        uint8_t lowByteLeft = lowByte(left);
        uint8_t highByteLeft = highByte(left);

        uint8_t lowByteRight = lowByte(right);
        uint8_t highByteRight = highByte(right);

        //placeholder until back sensor is implemented
        uint8_t lowByteBack = 0; 
        uint8_t highByteBack = 0;

        uint8_t checksum = lowByteFront + highByteFront + lowByteLeft + highByteLeft + lowByteRight + highByteRight + lowByteBack + highByteBack;

        Serial1.write(0xAA);// double start byte
        Serial1.write(0x55);

        Serial1.write(lowByteFront);
        Serial1.write(highByteFront);

        Serial1.write(lowByteLeft);
        Serial1.write(highByteLeft);

        Serial1.write(lowByteRight);
        Serial1.write(highByteRight);

        Serial1.write(lowByteBack);
        Serial1.write(highByteBack);

        Serial1.write(checksum);
        Serial1.write(0xBB);// End byte
    }
}