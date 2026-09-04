#pragma once

#include <Arduino.h>

struct SensorData {
    // ToF distances in millimeters, relative to the robot orientation.
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


struct OpenMVData {
    // OpenMV image coordinates in pixels; 250 represents “not detected”.
    uint16_t obstacleX = 250;
    uint16_t obstacleY = 250;
    uint16_t wallX = 250;
    uint16_t wallY = 250;
    uint16_t blueLineBottomY = 250;
    uint8_t obstacleColor = 0;
    
    bool obstacleDetected = false;
    bool wallDetected = false;
    bool blueLineDetected = false;

    // Raw received flags; the booleans are their decoded form.
    uint8_t flags = 0;
    uint32_t receivedAtMs = 0;
};
