/**
 * @file Move.cpp
 * @brief Methods to manage robot Mobility
 * 
 * @author Roy Barron / chaBotsMX
 * @date 20/04/26
 */


#include "Move.h"

Move::Move(){
    pinMode(INA,OUTPUT);
    pinMode(INB,OUTPUT);
    analogWriteFrequency(INA,20000);
    analogWriteFrequency(INB,20000);
    analogWriteResolution(8);
}
//This function takes a PWM value and sets the motor direction and speed accordingly.
void Move::driveAtPWM(int pwm){

    if(pwm > 0){
        digitalWrite(INA,HIGH);
        digitalWrite(INB,LOW);
        analogWrite(PWMPIN,pwm);
    }
    else if(pwm < 0){
        digitalWrite(INA,LOW);
        digitalWrite(INB,HIGH);
        analogWrite(PWMPIN,abs(pwm));
    }
}

//pwm ramp is for smooth acceleration, avoiding jerk in the robo.
//look for documentation in the applyPWMRamp function for more details.
int Move::applyPWMRamp(int targetPWM) {
    static float currentPWM = 0.0f;

    if (rampTimer > 0) {
        float dt = rampTimer / 1000.0f;

        float accelRate = 250.0f;
        float decelRate = 600.0f;

        bool changingDirection =
            (currentPWM > 0 && targetPWM < 0) ||
            (currentPWM < 0 && targetPWM > 0);

        if (changingDirection) {
            targetPWM = 0;
        }

        float delta = targetPWM - currentPWM;
        float maxStep = (fabs(targetPWM) > fabs(currentPWM) ? accelRate : decelRate) * dt;

        if (delta > maxStep) {
            currentPWM += maxStep;
        }
        else if (delta < -maxStep) {
            currentPWM -= maxStep;
        }
        else {
            currentPWM = targetPWM;
        }

        rampTimer = 0;
    }

    return (int)currentPWM;
}

//This function implements a simple PI controller to maintain the desired speed of the robot.
//look for documentation in the driveAtSpeed function for more details.
void Move::driveAtSpeed(float speed, float kp, float ki) {
    static float outPI = 0.0f;
    static float integral = 0.0f;

    float idealRevolutionPerSecond = (speed * CMSINMETER) / DISTANCEPERREVOLUTION;
    float estimatePWM = (idealRevolutionPerSecond / MAXRPS) * MAXPWM;

    encoderCounter = motorEncoder.read();

    if (encoderTimer > 100) {
        double encoderDelta = encoderCounter - lastEncoderCounter;
        float deltaTime = encoderTimer / 1000.0f;

        float wheelRevolution = encoderDelta / CPRPERSHAFTREVOLUTION;
        float distanceTraveled = wheelRevolution * DISTANCEPERREVOLUTION;   
        float actualSpeed = (distanceTraveled / CMSINMETER) / deltaTime;    
        float error = speed - actualSpeed;

        integral += error * deltaTime;
        outPI = kp * error + ki * integral;

        lastEncoderCounter = encoderCounter;
        encoderTimer = 0;
    }

    float out = estimatePWM + outPI;
    out = constrain(out, -255.0f, 255.0f);

    int rampedPWM = applyPWMRamp((int)out);
    driveAtPWM(rampedPWM);
}
