#include "TOF4Walls.h"
#include "Wire.h"
constexpr int LPN_FRONT = 2;
constexpr int LPN_BACK  = 3;
constexpr int LPN_LEFT  = 0;
constexpr int LPN_RIGHT = 1;


TOF4Walls tofs(Wire, LPN_FRONT, LPN_BACK, LPN_LEFT, LPN_RIGHT);

void setup() {
    pinMode(13,OUTPUT);
    digitalWrite(13, LOW);
    Serial.begin(115200);
    Serial1.begin(2000000, SERIAL_8N1, D7, D6);
    delay(1000);
    Serial.println("iniciando...");
    if (!tofs.begin(30)) {
        Serial.println("Error inicializando VL53L8CX");
        while (true) {  }
    }

    Serial.println("TOFs listos");
}

void loop() {
    
    Serial.println("actualizando");
    tofs.update();
if (tofs.hasFreshData(TOF4Walls::FRONT)) {
    uint16_t front = (uint16_t)tofs.getDistance(TOF4Walls::FRONT);
    uint16_t left  = (uint16_t)tofs.getDistance(TOF4Walls::LEFT);
    uint16_t right = (uint16_t)tofs.getDistance(TOF4Walls::BACK);

    Serial.print("F: ");
    Serial.print(front);
    Serial.print("  L: ");
    Serial.print(left);
    Serial.print("  R: ");
    Serial.println(right);

    Serial1.write(0xAA);                  // start byte

    Serial1.write(lowByte(front));
    Serial1.write(highByte(front));

    Serial1.write(lowByte(left));
    Serial1.write(highByte(left));

    Serial1.write(lowByte(right));
    Serial1.write(highByte(right));
}
    else {}
}