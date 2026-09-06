/**
 * @file TOF4Walls.cpp
 * @brief Methods to manage Time-of-Flight sensors for wall detection
 * 
 * @author Roy Barron / chaBotsMX
 * @date 20/04/26
 */


#include "TOF4Walls.h"
#include <algorithm>
//Selected zonea for detecntion of walls, 43 and 44 are the central zones for 8x8 resolution
const uint8_t TOF4Walls::CENTRAL_ZONES[NUM_LOOKUP_ZONES] = {35, 36};

TOF4Walls::TOF4Walls(SPIClass& spi,
                     int csFront,
                     int csRight,
                     int csLeft,
                     int csBack)
    : _spi(&spi),
      _front(&spi, csFront),
      _back(&spi, csBack),
      _left(&spi, csLeft),
      _right(&spi, csRight) {
    _csPins[FRONT] = csFront;
    _csPins[BACK] = csBack;
    _csPins[LEFT] = csLeft;
    _csPins[RIGHT] = csRight;

    for (int i = 0; i < 4; i++) {
        _distances[i] = -1;
        _fresh[i] = false;
        _lastStatus[i] = 255;
    }
}

bool TOF4Walls::begin(uint8_t freqHz) {
    // SPI chip select is active LOW. Deselect every sensor before starting the bus.
    for (int i = 0; i < 4; i++) {
        pinMode(_csPins[i], OUTPUT);
        digitalWrite(_csPins[i], HIGH);
    }

    _spi->begin();
    delay(10);

    // Configure every CS before communicating with any sensor.
    if (_front.begin() != 0);
    if (_right.begin() != 0);
    if (_left.begin() != 0);
    
    if (!initOne(_front, freqHz));
    if (!initOne(_right, freqHz)) ;
    if (!initOne(_left, freqHz));

    return true;
}

bool TOF4Walls::initOne(VL53L8CX& sensor, uint8_t freqHz) {
    if (sensor.init() != 0) return false;
    // The selected center zones require 8x8 resolution (maximum 15 Hz).
    if (sensor.set_resolution(VL53L8CX_RESOLUTION_8X8) != 0) return false;
    //continuous mode is required for the sensor to update its measurements
    //without needing a trigger
    if (sensor.set_ranging_mode(VL53L8CX_RANGING_MODE_CONTINUOUS) != 0) return false;
    if (sensor.set_ranging_frequency_hz(freqHz) != 0) return false;
    if (sensor.start_ranging() != 0) return false;
    delay(2);
    return true;
}

void TOF4Walls::update() {
    updateOne(_front, FRONT);
    updateOne(_right,  RIGHT);
    updateOne(_left,  LEFT);
}

void TOF4Walls::updateOne(VL53L8CX& sensor, uint8_t index) {
    uint8_t ready = 0;

    _fresh[index] = false;
    //if theres an error in checking data return to avoid using wrong data 
    if (sensor.check_data_ready(&ready) != 0) {
        return;
    }
    //if theres no new data return to avoid innecesary calculations  
    if (!ready) {
        return;
    }
  
    VL53L8CX_ResultsData results;
    //if theres an error in getting data return to avoid using wrong data
    if (sensor.get_ranging_data(&results) != 0) {
        return;
    }

    uint8_t chosenStatus = 255;
    //gets the information and store in the corresponding index, 
    _distances[index] = computeWallDistance(results, chosenStatus);
    _lastStatus[index] = chosenStatus;
    _fresh[index] = true;
}

int16_t TOF4Walls::computeWallDistance(const VL53L8CX_ResultsData& results, uint8_t& chosenStatus) const {
    int16_t valid[2];
    uint8_t count = 0;
    //status 255 means no valid measurement, 5 is the best, then 6 and 9 are acceptable, the rest are discarded
    chosenStatus = 255;

    for (uint8_t i = 0; i < NUM_LOOKUP_ZONES; i++) {
        uint8_t z = CENTRAL_ZONES[i];

      //check if the ligth reflected in that zone, target_detected 0 means no reflection or the same as no reading
        if (results.nb_target_detected[z] == 0) {
            continue;
        }

        uint8_t st = results.target_status[z];
        //check if the status is acceptable, if not, discard the reading
        if (!isUsableStatus(st)) {
            continue;
        }

        valid[count++] = results.distance_mm[z];

    }
    //if count is 0 means theres no reading, return -1 to indicate that.
    if (count == 0) {
        return -1;
    }

    return medianInt16(valid, count);
}

bool TOF4Walls::isUsableStatus(uint8_t status) {
    return (status == 5 || status == 6 || status == 9);
}

int16_t TOF4Walls::medianInt16(int16_t* values, uint8_t count) {
    // order the values to get median.
    std::sort(values, values + count);

    //Check if count is odd or even to return the median value, if its odd return the middle value, if its even return the average of the two middle values.
    if (count & 1) {
        return values[count / 2];
    }

    return (values[(count / 2) - 1] + values[count / 2]) / 2;
}

int16_t TOF4Walls::getDistance(Side side) const {
    return _distances[side];
}

bool TOF4Walls::hasFreshData(Side side) const {
    return _fresh[side];
}

uint8_t TOF4Walls::getLastStatus(Side side) const {
    return _lastStatus[side];
}
