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

Move::driveAtPWM(int pwm){

    if(pwm > 0){
        analogWrite(INA,pwm);
        digitalWrite(INB,0);
    }
    else if(pwm >= 0){
        digitalWrite(INA,0);
        analogWrite(INA,abs(pwm));
    }

}