/**
 * @file TOF4Walls.cpp
 * @brief Methods to manage Time-of-Flight sensors for wall detection
 * 
 * @author Roy Barron / chaBotsMX
 * @date 20/04/26
 */


#include "TOF4Walls.h"
#include <algorithm>
const uint8_t TOF4Walls::CENTRAL_ZONES[2] = { 43, 44};

TOF4Walls::TOF4Walls(TwoWire& wire,
                     int lpnFront,
                     int lpnBack,
                     int lpnLeft,
                     int lpnRight)
    : _wire(&wire),
      _front(&wire, lpnFront),
      _back(&wire, lpnBack),
      _left(&wire, lpnLeft),
      _right(&wire, lpnRight) {
    _lpnPins[0] = lpnFront;
    _lpnPins[1] = lpnBack;
    _lpnPins[2] = lpnLeft;
    _lpnPins[3] = lpnRight;

    for (int i = 0; i < 4; i++) {
        _distances[i] = -1;
        _fresh[i] = false;
        _lastStatus[i] = 255;
    }
}

bool TOF4Walls::begin(uint8_t freqHz) { 
    // Turn off every sensor 
    for (int i = 0; i < 4; i++) {
        pinMode(_lpnPins[i], OUTPUT);
        digitalWrite(_lpnPins[i], LOW);
    }
    Serial.println("Sensores apagados");
    delay(10);
    _wire->begin();
    delay(100);
    Serial.println("I2C iniciado");
    _wire->setClock(1000000); // cambiar si 1MHZ falla
    delay(10);
    // placeholder directions, one of them migth by the deault direction of the sensors
    //and may cause an error 
    const uint8_t addrs[4] = {0x01, 0x05, 0x08, 0xA0};
    //init sensor one by one
    Serial.println("Iniciando sensores");
    if (!initOne(_front, _lpnPins[0], addrs[0], freqHz)) return false;
    Serial.println("Sensor frontal listo");
    if (!initOne(_back,  _lpnPins[3], addrs[1], freqHz)) return false;
    Serial.println("Sensor derecho listo");
    if (!initOne(_left,  _lpnPins[2], addrs[2], freqHz)) return false;
    Serial.println("Sensor izquierdo listo");

    return true;
}

bool TOF4Walls::initOne(VL53L8CX& sensor, int lpnPin, uint8_t newAddress, uint8_t freqHz) {
   //Turn of the sensor by its LPN
    digitalWrite(lpnPin, HIGH);
    delay(1000);
    Serial.print("Iniciando sensor en pin ");    Serial.println(lpnPin);
    if (sensor.begin() != 0) return false;
    delay(1000);
    Serial.print("Sensor en pin ");    Serial.print(lpnPin); Serial.println(" iniciado");
    if (sensor.init() != 0) return false;
    delay(1000);
    Serial.print("Sensor en pin ");    Serial.print(lpnPin); Serial.println(" inited");
    if (sensor.set_i2c_address(newAddress) != 0) return false;
    Serial.print("Direccion I2C cambiada para sensor en pin ");    Serial.println(lpnPin);
    //16U is for 4x4 resolution, it can be used at 64U or 8x8, but its not necessary  
    if (sensor.set_resolution(64U) != 0) return false;
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
    updateOne(_back,  BACK);
    updateOne(_left,  LEFT);
    updateOne(_right, RIGHT);
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

    for (uint8_t i = 0; i < 2; i++) {
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