/**
 * @file Move.h
 * @brief Methods to manage robot mobility.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 20/04/26
 */

#ifndef MOVE_H
#define MOVE_H


#include <MotorController.h>
#include <cmath>

struct MoveProfile {
    float VMax; // Maximum speed in cm/s
    float VFinal; // Final speed in cm/s
    float A;    // Acceleration in cm/s^2
    float D;    // Deceleration in cm/s^2
    float VInitial; // Initial speed in cm/s

};
class Move {
public:
    Move();
  
    void updateCM();
    void setTask(float distanceCM, float speedCMperS, float accelerationCMperS2, float decelerationCMperS2, float initialSpeedCMperS, float finalSpeedCMperS);
    void driveAtPWM(int pwm);
    void driveAtSpeed(float speedCMperS, float kp, float ki, float actualCM);
    int calculateProfileSpeed();
    int getCurrentSpeed() const {
        return robotSpeed;
    }
private:
    MotorController controller;
    elapsedMillis speedTimer;
    MoveProfile profile;
    const int MIN_SPEED = 10; // Minimum speed in cm/s
    float CMProfileSpeed = 0.0f;
    uint32_t lastProfileUpdateUs = 0;
    long int encoderCounter = 0;
    long int lastEncoderCounter = 0;
    int CMtarget = 0;
    int CMSpeedTarget = 0;
    int CMcurrent = 0;
    int lastCMcurrent = 0;
    int robotSpeed = 0;
};

#endif