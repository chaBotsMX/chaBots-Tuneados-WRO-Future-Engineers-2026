#include <Arduino.h>
#include "Xiao_UART.h"
#include <HardwareSerial.h>
#include "AckermannController.h"
#include <Adafruit_NeoPixel.h>
#include <Move.h>
#include "IMU.h"

Adafruit_NeoPixel pixels(1, 22, NEO_GRB + NEO_KHZ800); // 1 LED on pin 6

AckermannController ackermann;
IMU imu;

Xiao_UART xiao(Serial5, 2000000);

Move move;

SensorData data;

SensorData validData;

#define BUTTON 10
#define BUZZER 11

int imuSetPoint = 0; // Desired heading in degrees
int imuError = 0;    // Error between desired and actual heading
int desiredDistance = 200; // Desired distance to the wall in mm


float wrap180(float angle)
{
    while (angle > 180.0f) {
        angle -= 360.0f;
    }

    while (angle < -180.0f) {
        angle += 360.0f;
    }

    return angle;
}

float stanleySteering(
    float wallError,
    float speed,
    float k
) {
    constexpr float HEADING_K = 0.5f;

    float angularError =
        wrap180(imuError);

    float lateralCorrection =
        degrees(
            atan2(
                k * wallError,
                speed + 1.0f
            )
        );

    return
        HEADING_K * angularError +
        lateralCorrection;
}

void updateSensors(){
    if (xiao.available()) {

    data = xiao.getData();
    if(data.front < 4000 ) {
      validData.front = data.front;
    }
    if(data.left < 4000 ) {
      validData.left = data.left;
    }
    if(data.right < 4000 ) {
      validData.right = data.right;
    }
    if(data.back < 4000 ) {
      validData.back = data.back;
    }
  }

}


void setup() {
  // put your setup code here, to run once:
  pixels.begin();
  pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // Set the LED color to red
  pixels.show(); // Update the LED to show the color
  Serial.begin(115200);
  Serial4.begin(115200); // IMU
  Serial5.begin(2000000); // RX, TX

  imu.begin(Serial4);

  delay(1000); // Wait for Serial to initialize
  pinMode(13, OUTPUT); // Set pin 13 as output for the LED
  pinMode(BUTTON, INPUT); // Set pin 10 as input for the button
  pinMode(BUZZER, OUTPUT); // Set pin 11 as output for the buzzer
  ackermann.begin();
  move.setTask(2000, 50,10,10,0,0); // Example: Move 100 cm at 50 cm/s with acceleration and deceleration of 100 cm/s^2
  validData.front = 3000;
  validData.left = 3000;
  validData.right = 3000;



  bool initialized = false;
  while (initialized == false) {
    xiao.readData();
    if (xiao.available()) {
      data = xiao.getData();
      if(data.front < 4000 ) {
        validData.front = data.front;
      }
      if(data.left < 4000 ) {
        validData.left = data.left;
      }
      if(data.right < 4000 ) {
        validData.right = data.right;
      }
      if(data.back < 4000 ) {
        validData.back = data.back;
      }
      pixels.setPixelColor(0, pixels.Color(0,255, 0)); // Set the LED color to green
      pixels.show(); // Update the LED to show the color
    }
    if (digitalRead(BUTTON) == HIGH) {
      initialized = true;
      pixels.setPixelColor(0, pixels.Color(0, 0, 255)); // Set the LED color to blue
      pixels.show(); // Update the LED to show the color
      tone(BUZZER, 1000, 500); // Play a tone on the buzzer
      delay(500); // Wait for the tone to finish
    }
  }
}

void followWallUntilEdge(){
  float angle = stanleySteering(validData.left - desiredDistance, float(move.getCurrentSpeed()), 0.002f);
  Serial.print("Steering Angle: " + String(angle) + " IMU Error: " + String(imuError) + " Wall Error: " + String(validData.left - desiredDistance) + " Speed: " + String(move.getCurrentSpeed()));
  ackermann.setSteeringAngle(angle);
  move.updateCM(); 
}

void loop() {
  imu.update();
  imuError = imuSetPoint - imu.getYaw();
 // Serial.println("IMU Yaw: " + String(imu.getYaw()) + " IMU Error: " + String(imuError));
  digitalWrite(13, HIGH); // Turn on the LED
  xiao.readData();
  //ackermann.setSteeringAngle(0.0f); // Example: Set steering angle to 15 degrees
  updateSensors();

  followWallUntilEdge();
  if (validData.front < 200) {
    // Stop the robot
    while (true) {
      move.driveAtPWM(0); // Stop the motors
      ackermann.setSteeringAngle(0.0f); // Center the steering
    }
  }

}

