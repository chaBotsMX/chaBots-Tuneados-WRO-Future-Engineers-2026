/**
 * @file TOF4Walls.h
 * @brief Header file for managing Time-of-Flight sensors for wall detection
 * 
 * @author Roy Barron / chaBotsMX
 * @date 20/04/26
 */



#ifndef TOF4WALLS_H
#define TOF4WALLS_H

#include <Arduino.h>
#include <Wire.h>
#include <vl53l8cx.h>
//Define i2c clock speed
#define I2C_SPEED 1000000
class TOF4Walls {
public:
    enum Side : uint8_t {
        FRONT = 0,
        BACK  = 1,
        LEFT  = 2,
        RIGHT = 3
    };

    TOF4Walls(TwoWire& wire,
              int lpnFront,
              int lpnBack,
              int lpnLeft,
              int lpnRight);

    bool begin(uint8_t freqHz = 60);
    void update();

    int16_t getDistance(Side side) const;
    bool hasFreshData(Side side) const;
    uint8_t getLastStatus(Side side) const;

private:
    static const uint8_t CENTRAL_ZONES[2];

    TwoWire* _wire;
    int _lpnPins[4];

    VL53L8CX _front;
    VL53L8CX _back;
    VL53L8CX _left;
    VL53L8CX _right;

    int16_t _distances[4];
    bool _fresh[4];
    uint8_t _lastStatus[4];

    bool initOne(VL53L8CX& sensor, int lpnPin, uint8_t newAddress, uint8_t freqHz);
    void updateOne(VL53L8CX& sensor, uint8_t index);

    int16_t computeWallDistance(const VL53L8CX_ResultsData& results, uint8_t& chosenStatus) const;

    static bool isUsableStatus(uint8_t status);
    static int16_t medianInt16(int16_t* values, uint8_t count);
};

#endif