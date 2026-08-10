#include <Arduino.h>
#include "MotorController.h"

MotorController motorCompileCheck;

void setup() {
    motorCompileCheck.begin();
    (void)motorCompileCheck.getSpeedcms();
}

void loop() {}
