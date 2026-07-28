/**
 * @file MotorController.h
 * 
 * @author Roy Ivan Barron Martinez / chaBotsMX
 * @date 28/04/26
 */

#ifndef MOTOR_CONTROLLER_H
#define MOTOR_CONTROLLER_H

#include <Arduino.h>
#include <Encoder.h>

#define INA         3
#define INB         1
#define PWM         2
#define ENCA        9
#define ENCB        8

// max pwm to prevent overheating the motor, can be adjusted based on testing.
#define MAX_PWM 200

// number of ticks that euals one revolution of the motor shaft, obtained from pololu datasheet.
#define CPR_PER_SHAFT_REVOLUTION 979.6128f

// maximum revolutions per second.
#define MAX_RPS 8.3333f

// Distance per revoliution in cm, obtained from measurements of the wheel diameter.
#define DISTANCE_PER_REVOLUTION 13.037f   // cm

// distance in mm per tick, calculated from the distance per revolution and the counts per revolution.
const float MM_PER_TICK = (DISTANCE_PER_REVOLUTION * 10.0f) / CPR_PER_SHAFT_REVOLUTION;


class MotorController {
  public:
    MotorController();
    void begin();
    void motorDrivebyPWM(int pwm);
    void resetTicks();
    void brake();
    long int getTicks();
    float getDistanceMM();
    long int getAbsoluteTicks();
    float getAbsoluteDistanceMM();
  private:
    Encoder motor;
    long int counterTicks = 0;
};

#endif