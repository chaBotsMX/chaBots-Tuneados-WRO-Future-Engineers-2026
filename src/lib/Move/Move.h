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

struct MotionTaskConfig;
struct SpeedControlConfig;

struct MoveProfile {
    float VMax; // Maximum speed in cm/s
    float VFinal; // Final speed in cm/s
    float A;    // Acceleration in cm/s^2
    float D;    // Deceleration in cm/s^2
    float VInitial; // Initial speed in cm/s

};
// Manages distance profiles and motor commands. updateCM() advances the
// profile, brakes when it finishes, and returns true only on completion.
class Move {
public:
    Move();
    MotorController controller;
    void begin();
    bool updateCM();
    void setTask(float distanceCM, float speedCMperS, float accelerationCMperS2, float decelerationCMperS2, float initialSpeedCMperS, float finalSpeedCMperS);
    void setTask(const MotionTaskConfig& task);
    void driveAtPWM(int pwm);
    // Proportional speed control; ki is reserved and not used yet.
    void driveAtSpeed(float speedCMperS, float kp, float ki, float actualCM);
    void driveAtSpeed(const SpeedControlConfig& control, float actualCM);
    int calculateProfileSpeed();
    int getCurrentSpeed() const {
        return robotSpeed;
    }
private:

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
