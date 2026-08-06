/**
 * @file Move.cpp
 * @brief Methods to manage robot Mobility
 * 
 * @author Roy Barron / chaBotsMX
 * @date 20/04/26
 */


#include "Move.h"


Move::Move(){
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

void Move::driveAtSpeed(float speedCMperS, float kp, float ki, float actualCM){
    int actualSpeedPWM = (actualCM - lastCMcurrent) / (speedTimer / 1000.0f); // Calculate actual speed in cm/s
    int speedError = actualSpeedPWM - speedCMperS; // Calculate speed error
    int pwmOutput = kp * speedError; // Proportional control
    pwmOutput = constrain(pwmOutput, -MAX_PWM, MAX_PWM); // Constrain the PWM output to prevent overheating
    controller.motorDrivebyPWM(pwmOutput);
    lastCMcurrent = actualCM;
}

void Move::updateCM(){
    CMcurrent = abs(controller.getDistanceMM()) / 10.0f; // Convert mm to cm
    Serial.print("Current CM: ");
    Serial.println(CMcurrent);
    if (CMcurrent < CMtarget) {
        float targetDistanceTraveledPercentage = (CMcurrent / CMtarget) * 100.0f;
        int profileSpeed = this->calculateProfileSpeed();
        profileSpeed = constrain(profileSpeed, MIN_SPEED, CMSpeedTarget); //minimum positive output to avoid motor stuck in 0 at init of the task.
        this->driveAtSpeed(profileSpeed, 5, 0, CMcurrent);
    } else {
        // Stop the motor
        controller.brake();
    }
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