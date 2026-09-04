/**
 * @file IMU.h
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#ifndef IMU_H
#define IMU_H

#include <Adafruit_BNO08x.h>
#include <Arduino.h>
#include "Utils.h"

class IMU {
public:
    IMU();
    bool begin(HardwareSerial& imuPort);
    // Must be called every loop before reading yaw or error.
    bool update();
    float getError();
    // Sets the target heading; getError() returns the error relative to it.
    void setSetPoint(float newError);
    float getYaw();
    float setPoint;
  private:
    Adafruit_BNO08x bno08x;
    sh2_SensorValue_t sensorValue;
    float yaw;
    float error;

};

#endif
