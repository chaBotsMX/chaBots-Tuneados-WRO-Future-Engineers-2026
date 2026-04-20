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
#include <encoder>
//define VNH7070 H bridge control pins.
#define INA  10
#define INB  11
//define motor encoder reading pins.
#define ENCA 12
#define ENCB 13

//this fuction create an object of class encoder, this will auto update the encoder counters using interrupts.

Encoder motorEncoder(7,8);

class Move {
public:
    Move();
    void driveAtPWM(int pwm);
private:

};

#endif