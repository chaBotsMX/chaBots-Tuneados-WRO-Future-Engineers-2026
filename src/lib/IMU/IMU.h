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
    bool update();
    float getError();
    void setSetPoint(int newError);
    float getYaw();
  private:
    Adafruit_BNO08x bno08x;
    sh2_SensorValue_t sensorValue;
    float yaw;
    float error;
    float setPoint;
};

#endif