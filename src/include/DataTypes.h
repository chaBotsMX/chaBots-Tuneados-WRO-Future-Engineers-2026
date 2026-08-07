#ifndef DataTypes_h
#define DataTypes_h

#include <Arduino.h>

struct SensorData {
    uint16_t front;
    uint16_t left;
    uint16_t right;
    uint16_t back;
};

#endif