/**
 * @file MotorController.cpp
 * @brief This class contain Robots logic drive motor and get inputs from encoders.
 * @author Roy Ivan Barron Martinez / chaBotsMX
 * @date 28/04/26
 */
#include "MotorController.h"

//initialization of the encoder object
MotorController::MotorController() : motor(ENCA, ENCB){
}

//initialization of the pins
void MotorController::begin(){
  pinMode(INA, OUTPUT);
  pinMode(INB, OUTPUT);
  pinMode(PWM, OUTPUT);
  pinMode(ENCA, INPUT);
  pinMode(ENCB, INPUT);
  analogWriteFrequency(PWM, 20000); // set PWM frequency to 20 kHz.
}


// this function returns the number of ticks since the last reset
long int MotorController::getTicks(){
  return motor.read() - counterTicks;
}


// this function returns the distance in mm since the last reset
float MotorController::getDistanceMM(){
  return (motor.read() - counterTicks) * MM_PER_TICK;
}

// this function returns the absolute number of ticks since the beginning
long int MotorController::getAbsoluteTicks(){
  return motor.read();
}

// this function returns the absolute distance in mm since the beginning
float MotorController::getAbsoluteDistanceMM(){
  return motor.read() * MM_PER_TICK;
}


// this function controls the motor speed and direction using PWM
void MotorController::motorDrivebyPWM(int pwm){
    pwm = constrain(pwm, -MAX_PWM, MAX_PWM);
  if(pwm > 0){
    digitalWrite(INA, HIGH);
    digitalWrite(INB, LOW);
    analogWrite(PWM, pwm);
  } else if(pwm < 0){
    digitalWrite(INA, LOW);
    digitalWrite(INB, HIGH);
    analogWrite(PWM, -pwm);
  } else {
    digitalWrite(INA, LOW);
    digitalWrite(INB, LOW);
    analogWrite(PWM, 0);
  }
}

// this function brake the motor by setting both INA and INB to HIGH and applying maximum PWM
void MotorController::brake(){
  digitalWrite(INA, HIGH);
  digitalWrite(INB, HIGH);
  analogWrite(PWM, MAX_PWM);
}

// this functiion reset ticks.
void MotorController::resetTicks(){
  counterTicks = motor.read();
}
