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
class AckermannController;

// Settings for the new executeMoving() controller only. Legacy control is unchanged.
struct MovePIDConfig {
    float kp = 5.0f;
    float ki = 2.0f;
    float kd = 0.02f;
    uint16_t sampleTimeMs = 20;
    int maxPWM = MAX_PWM;
    // Obstacles currently uses negative PWM to drive forward. Set encoderForwardSign
    // to +1 if the raw encoder count increases when the robot moves forward.
    int pwmForwardSign = -1;
    int encoderForwardSign = -1;
};

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
    explicit Move(const MovePIDConfig& config);
    Move(AckermannController& steering, const MovePIDConfig& config = MovePIDConfig());
    MotorController controller;
    void begin();
    bool updateCM();
    void setTask(float distanceCM, float speedCMperS, float accelerationCMperS2, float decelerationCMperS2, float initialSpeedCMperS, float finalSpeedCMperS);
    void setTask(const MotionTaskConfig& task);
    void driveAtPWM(int pwm);
    // Call every loop, without other motor commands: ramp the current PWM to zero,
    // then brake. Returns true when the ramp ends (not a measured standstill).
    bool smoothBrake(uint32_t durationMs = 1000);
    // Proportional speed control; ki is reserved and not used yet.
    void driveAtSpeed(float speedCMperS, float kp, float ki, float actualCM);
    void driveAtSpeed(const SpeedControlConfig& control, float actualCM);
    int calculateProfileSpeed();
    int getCurrentSpeed() const {
        return robotSpeed;
    }
    // New non-blocking API: positive cm = forward, negative cm = reverse.
    // speedCMperS is a positive speed in cm/s, NOT a PWM value.
    // Start each command once. A new command replaces the pending movement.
    bool moveByCm(float cm, float speedCMperS);
    bool turnByCm(float cm, float speedCMperS, float steeringAngleDeg);
    // Callback: false while moving; true when idle or after reaching the distance.
    bool executeMoving();
    void cancelMoving();
    void setSteeringController(AckermannController& steering);
    // Configure after begin(); cancels a pending movement and resets its PID.
    void configureMotionPID(const MovePIDConfig& config);
    bool isMoving() const { return motionActive; }
    float getMotionSpeedCmS() const { return motionSpeedCmS; }
    float getMotionDistanceCm() const { return motionDistanceCm; }
private:

    bool smoothBrakeActive = false;
    int smoothBrakeInitialPWM = 0;
    uint32_t smoothBrakeStartedMs = 0;

    bool startMotion(float cm, float speedCMperS, float steeringAngleDeg);
    AckermannController* motionSteering = nullptr;
    MovePIDConfig motionPID;
    bool motionActive = false;
    int motionDirection = 1;
    long motionStartTicks = 0;
    long motionLastTicks = 0;
    uint32_t motionLastUpdateUs = 0;
    float motionTargetCm = 0.0f;
    float motionTargetSpeedCmS = 0.0f;
    float motionDistanceCm = 0.0f;
    float motionSpeedCmS = 0.0f;
    float motionLastSpeedCmS = 0.0f;
    float motionIntegralPWM = 0.0f;

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
