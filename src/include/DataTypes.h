#pragma once

#include <Arduino.h>

struct SensorData {
    uint16_t front = 0; 
    uint16_t left = 0;
    uint16_t right = 0;
    uint16_t back = 0;
};

struct SteeringCalibration {
    float ackermannAngle;
    float servoOffset;
};

enum class DIRECTIONS {
    CLOCKWISE,
    COUNTERCLOCKWISE
};


