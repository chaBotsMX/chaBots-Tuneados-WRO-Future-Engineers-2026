/**
 * @file IMU.cpp
 * @brief Methods to get robot's heading using the BNO08x. Based in Adafruit's library
 * 
 * @author Alfonso De Anda / chaBotsMX
 * @date 10/02/26
 */

#include "IMU.h"

IMU::IMU(){
  yaw = 0.0;
}

bool IMU::begin(HardwareSerial& imuPort){
  if (!bno08x.begin_UART(&imuPort)) {
    return false;
  }
  bno08x.enableReport(SH2_GAME_ROTATION_VECTOR);
  return true;
}

bool IMU::update(){
  if (!bno08x.getSensorEvent(&sensorValue)) return false;
  
  if (sensorValue.sensorId == SH2_GAME_ROTATION_VECTOR) {
    float qr = sensorValue.un.gameRotationVector.real;
    float qi = sensorValue.un.gameRotationVector.i;
    float qj = sensorValue.un.gameRotationVector.j;
    float qk = sensorValue.un.gameRotationVector.k;
    
    yaw = atan2(2.0 * (qi * qj + qk * qr), (sq(qr) + sq(qi) - sq(qj) - sq(qk))) * RAD_TO_DEG;
    
  }
  return true;
}

float IMU::getYaw(){
  return yaw;
}