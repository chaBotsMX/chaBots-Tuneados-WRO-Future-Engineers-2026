/**
 * @file Ackermann.h
 * @brief Header file for managing Ackermann steering geometry
 * 
 * @author Roy Barron / chaBotsMX
 * @date 20/04/26
 */

#ifndef ACKERMANN_H
#define ACKERMANN_H

#include <Arduino.h>

#define DEFAULT_GRADE_ANGLE 90.0f
#define MAX_STEERING_ANGLE 20.0f

#define SERVO_PIN 12

class Ackermann {
public:
    Ackermann(float initialGradeAngle = DEFAULT_GRADE_ANGLE);
    void setSteeringAngle(float desiredGradeAngle);
};
   

#endif // ACKERMANN_H