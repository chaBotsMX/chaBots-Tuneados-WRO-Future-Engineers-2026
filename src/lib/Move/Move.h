/**
 * @file Move.h
 * @brief Methods to manage robot mobility.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 20/04/26
 */

#ifndef MOVE_H
#define MOVE_H

//this option optimize the use of interrupts in teensy and improve performance not using attachinterrupt
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Arduino.h>
#include <Encoder.h>
//define VNH7070 H bridge control pins.
#define INA  10
#define INB  11
#define PWMPIN 9
//define motor encoder reading pins.
#define ENCA 12
#define ENCB 13
#define PI 3.14159265
#define MAXPWM 255
#define CMSINMETER 100 
#define CPRPERSHAFTREVOLUTION 979.6128
//calculated using motor nominal max RPM of 500 divided by 60 to get RPS.
#define MAXRPS 8.3333
//calculated using Pi * diameter of the wheel.
#define DISTANCEPERREVOLUTION 13.037 
//this fuction create an object of class encoder, this will auto update the encoder counters using interrupts.
Encoder motorEncoder(7,8);
elapsedMillis encoderTimer;
elapsedMillis rampTimer;
class Move {
public:
    Move();
    void driveAtPWM(int pwm);
    void driveAtSpeed(float speed, float kp, float ki);
    int applyPWMRamp(int targetPWM);
private:
    long int encoderCounter = 0;
    long int lastEncoderCounter = 0;
};

#endif