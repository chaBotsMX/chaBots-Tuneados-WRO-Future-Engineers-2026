/**
 * @file Move.cpp
 * @brief Methods to manage robot Mobility
 * 
 * @author Roy Barron / chaBotsMX
 * @date 20/04/26
 */


#include "Move.h"
#include "ControlValues.h"
#include "AckermannController.h"


Move::Move(){
}

void Move:: begin(){
    controller.begin();
}

void Move::setTask(float distanceCM, float speedCMperS, float accelerationCMperS2, float decelerationCMperS2, float initialSpeedCMperS, float finalSpeedCMperS)
{
    controller.resetTicks();

    CMtarget = fabsf(distanceCM);
    CMSpeedTarget = fabsf(speedCMperS);

    CMcurrent = 0.0f;
    lastCMcurrent = 0.0f;
    CMProfileSpeed = 0.0f;

    lastProfileUpdateUs = micros();
    speedTimer = 0;
    profile.A = fabsf(accelerationCMperS2);
    profile.D = fabsf(decelerationCMperS2);
    profile.VMax = fabsf(speedCMperS);
    profile.VInitial = fabsf(initialSpeedCMperS);
    profile.VFinal = fabsf(finalSpeedCMperS);
}

void Move::setTask(const MotionTaskConfig& task)
{
    setTask(task.distanceCm, task.maxSpeedCmS,
            task.accelerationCmS2, task.decelerationCmS2,
            task.initialSpeedCmS, task.finalSpeedCmS);
}

void Move::driveAtSpeed(float speedCMperS, float kp, float ki, float actualCM){
    int actualSpeedPWM = (actualCM - lastCMcurrent) / (speedTimer / 1000.0f); // Calculate actual speed in cm/s
    robotSpeed = actualSpeedPWM;
    int speedError = actualSpeedPWM - speedCMperS; // Calculate speed error
    int pwmOutput = kp * speedError; // Proportional control
    pwmOutput = constrain(pwmOutput, -MAX_PWM, MAX_PWM); // Constrain the PWM output to prevent overheating
    controller.motorDrivebyPWM(pwmOutput);
    lastCMcurrent = actualCM;
}

void Move::driveAtSpeed(const SpeedControlConfig& control, float actualCM){
    driveAtSpeed(control.targetSpeedCmS, control.kp, control.ki, actualCM);
}

bool Move::updateCM(){
    CMcurrent = abs(controller.getDistanceMM()) / 10.0f; // Convert mm to cm
    Serial.print("Current CM: ");
    Serial.println(CMcurrent);
    if (CMcurrent < CMtarget) {
        int profileSpeed = this->calculateProfileSpeed();
        profileSpeed = constrain(profileSpeed, MIN_SPEED, CMSpeedTarget); //minimum positive output to avoid motor stuck in 0 at init of the task.
        this->driveAtSpeed(profileSpeed, 5, 0, CMcurrent);
        return false;
    } 
    // Stop the motor
    controller.brake();
    return true;
    
}

//This function takes a PWM value and sets the motor direction and speed accordingly.
void Move::driveAtPWM(int pwm){
    controller.motorDrivebyPWM(pwm);
}
// vobjetivo ​= min(vmax​,v02​+2adrecorrida​​,vf2​+2ddrestante​​)

int Move::calculateProfileSpeed(){
    float VoptimalActual = sqrt(pow(profile.VInitial, 2) + 2 * profile.A * CMcurrent);
    float VoptimalFinal = sqrt(pow(profile.VFinal, 2) + 2 * profile.D * (CMtarget - CMcurrent));
    float CMProfileSpeed = fmin(profile.VMax, fmin(VoptimalActual, VoptimalFinal));
    return int(CMProfileSpeed);
}

// New distance commands have their own state and PID; the legacy methods above
// keep their original behavior. Do not run both controllers at the same time.
Move::Move(const MovePIDConfig& config) : motionPID(config) {}

Move::Move(AckermannController& steering, const MovePIDConfig& config)
    : motionSteering(&steering), motionPID(config) {}

void Move::setSteeringController(AckermannController& steering) {
    motionSteering = &steering;
}

void Move::configureMotionPID(const MovePIDConfig& config) {
    cancelMoving();
    motionPID = config;
}

void Move::cancelMoving() {
    motionActive = false;
    motionIntegralPWM = 0.0f;
    motionLastSpeedCmS = 0.0f;
    motionSpeedCmS = 0.0f;
    controller.brake();
}

bool Move::moveByCm(float cm, float speedCMperS) {
    return startMotion(cm, speedCMperS, 0.0f);
}

bool Move::turnByCm(float cm, float speedCMperS, float steeringAngleDeg) {
    if (motionSteering == nullptr) {
        cancelMoving();
        return false;
    }
    return startMotion(cm, speedCMperS, steeringAngleDeg);
}

bool Move::startMotion(float cm, float speedCMperS, float steeringAngleDeg) {
    cancelMoving();
    motionDistanceCm = 0.0f;
    if (!std::isfinite(cm) || !std::isfinite(speedCMperS) || speedCMperS <= 0.0f ||
        !std::isfinite(steeringAngleDeg) ||
        !std::isfinite(motionPID.kp) || motionPID.kp < 0.0f ||
        !std::isfinite(motionPID.ki) || motionPID.ki < 0.0f ||
        !std::isfinite(motionPID.kd) || motionPID.kd < 0.0f ||
        motionPID.sampleTimeMs == 0 || motionPID.maxPWM <= 0 || motionPID.maxPWM > MAX_PWM ||
        (motionPID.pwmForwardSign != 1 && motionPID.pwmForwardSign != -1) ||
        (motionPID.encoderForwardSign != 1 && motionPID.encoderForwardSign != -1)) {
        return false;
    }
    if (cm == 0.0f) {
        return true;
    }
    if (motionSteering != nullptr) {
        motionSteering->setSteeringAngle(steeringAngleDeg);
    }
    motionDirection = cm > 0.0f ? 1 : -1;
    motionTargetCm = fabsf(cm);
    motionTargetSpeedCmS = speedCMperS;
    // A private absolute baseline preserves resetTicks() and the race's odometry.
    motionStartTicks = controller.getAbsoluteTicks();
    motionLastTicks = motionStartTicks;
    motionLastUpdateUs = micros();
    motionActive = true;
    return true;
}

namespace {
// Teensy encoder counts are signed 32-bit values. Unsigned subtraction also
// handles a counter rollover during a movement without signed overflow.
float motionTickDeltaCm(long current, long previous) {
    const int32_t ticks = static_cast<int32_t>(
        static_cast<uint32_t>(current) - static_cast<uint32_t>(previous));
    return static_cast<float>(ticks) * (MM_PER_TICK / 10.0f);
}
}

bool Move::executeMoving() {
    if (!motionActive) {
        return true;
    }

    const long ticks = controller.getAbsoluteTicks();
    motionDistanceCm = motionTickDeltaCm(ticks, motionStartTicks) * motionPID.encoderForwardSign;
    if (motionDistanceCm * motionDirection >= motionTargetCm) {
        cancelMoving();
        return true;
    }

    const uint32_t now = micros();
    const uint32_t elapsedUs = now - motionLastUpdateUs;
    if (elapsedUs < static_cast<uint32_t>(motionPID.sampleTimeMs) * 1000U) {
        return false;
    }

    const float dt = elapsedUs / 1000000.0f;
    motionSpeedCmS = motionTickDeltaCm(ticks, motionLastTicks) * motionPID.encoderForwardSign / dt;
    // Work along the requested direction so the same PID works in reverse.
    const float speed = motionSpeedCmS * motionDirection;
    const float error = motionTargetSpeedCmS - speed;
    const float proportional = motionPID.kp * error;
    // Derivative on measured speed avoids a kick from a new speed target.
    const float derivative = -motionPID.kd * (speed - motionLastSpeedCmS) / dt;
    const float limit = static_cast<float>(motionPID.maxPWM);
    const float candidateIntegral = fmaxf(-limit, fminf(limit,
        motionIntegralPWM + motionPID.ki * error * dt));
    const float candidateOutput = proportional + candidateIntegral + derivative;
    // Anti-windup: only integrate at saturation if the error brings PWM back
    // into range. At low PWM, zero encoder speed keeps increasing the integral.
    if ((candidateOutput <= limit || error < 0.0f) &&
        (candidateOutput >= 0.0f || error > 0.0f)) {
        motionIntegralPWM = candidateIntegral;
    }
    const float output = fmaxf(0.0f, fminf(limit,
        proportional + motionIntegralPWM + derivative));
    controller.motorDrivebyPWM(static_cast<int>(lroundf(output)) *
                               motionDirection * motionPID.pwmForwardSign);

    motionLastTicks = ticks;
    motionLastUpdateUs = now;
    motionLastSpeedCmS = speed;
    return false;
}

bool Move::smoothBrake(uint32_t durationMs) {
    if (!smoothBrakeActive) {
        smoothBrakeInitialPWM = controller.lastPwm;
        smoothBrakeStartedMs = millis();
        smoothBrakeActive = true;
        // Take over a pending distance movement without applying its abrupt brake.
        motionActive = false;
        motionIntegralPWM = 0.0f;
    }

    const uint32_t elapsedMs = millis() - smoothBrakeStartedMs;
    if (elapsedMs >= durationMs || smoothBrakeInitialPWM == 0) {
        controller.brake();
        smoothBrakeActive = false;
        return true;
    }

    const float remaining = 1.0f - static_cast<float>(elapsedMs) / durationMs;
    controller.motorDrivebyPWM(static_cast<int>(smoothBrakeInitialPWM * remaining));
    return false;
}
