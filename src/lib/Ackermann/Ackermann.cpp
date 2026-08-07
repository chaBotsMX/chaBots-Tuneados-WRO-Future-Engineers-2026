/**
 * @file Ackermann.cpp
 * @brief Implementation file for managing Ackermann steering geometry
 * 
 * @author Roy Barron / chaBotsMX
 * @date 20/04/26
 */

 #include "Ackermann.h"


//after calculate the max steering angle using the bicicle gemotery, we found that our robot can steer up to
//25.94 grades, using this we can aproximate the output to turn to any desired angle, as we know that 
// this 25.94 grades are achived turning 20 degrees of the servo.

 void Ackermann::setSteeringAngle(float desiredGradeAngle) {
     // Clamp the desired grade angle to the maximum steering angle
     if (desiredGradeAngle > MAX_STEERING_ANGLE) {
         desiredGradeAngle = MAX_STEERING_ANGLE;
     } else if (desiredGradeAngle < -MAX_STEERING_ANGLE) {
         desiredGradeAngle = -MAX_STEERING_ANGLE;
     }

     // Map the desired grade angle to the servo's PWM range
    
 }