#include <Arduino.h>
#include "Xiao_UART.h"
#include <HardwareSerial.h>

Xiao_UART xiao(Serial5, 2000000);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial5.begin(2000000); // RX, TX
  delay(1000); // Wait for Serial to initialize
  pinMode(13, OUTPUT); // Set pin 13 as output for the LED
}

void loop() {
  xiao.readData();

  if (xiao.available()) {
    SensorData data = xiao.getData();
    Serial.print("Front: ");
    Serial.print(data.front);
    Serial.print(" Left: ");
    Serial.print(data.left);
    Serial.print(" Right: ");
    Serial.print(data.right);
    Serial.print(" Back: ");
    Serial.println(data.back);  
  }
}

