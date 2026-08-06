#include <Arduino.h>
#include <Move.h>

Move motor;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  motor.setTask(200,30,30,20,0,0); 
}

void loop() {
  motor.updateCM();

}

