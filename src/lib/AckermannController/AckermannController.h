/**
 * @file AckermannController.h
 *
 * @author Roy Barron / chaBotsMX
 * @date 08/07/26
 */

#ifndef ACKERMANNCONTROLLER_H
#define ACKERMANNCONTROLLER_H

#include <Arduino.h>
#include <Servo.h>
#include "DataTypes.h"

#define MAX_ACKERMANN_ANGLE 25.94f
#define MAX_SERVO_OFFSET    20.0f
#define IDLE_STEERING_ANGLE 80.0f

#define SERVO_PIN 22

class AckermannController {
public:
    AckermannController();
    float lastSteeringAngleDeg = 0.0f;
    void begin();
    void setSteeringAngle(float angle);
    void steerByRawAngle(float rawAngle);

private:
    Servo steeringServo;

    float interpolateServoOffset(float ackermannAngle);

    static constexpr int MAP_SIZE = 5;

    static constexpr SteeringCalibration steeringTable[MAP_SIZE] = {
        { 0.00f,  0.0f },
        { 5.00f,  5.0f },
        {11.92f, 10.0f },
        {18.17f, 15.0f },
        {25.94f, 20.0f }
    };
};

#endif
