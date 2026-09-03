/**
 * @file HCI.h
 * @brief library to manage user interaction with the robot.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 11/08/26
*/

#ifndef HCI_H
#define HCI_H

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define BUTTON 10
#define BUZZER 23
#define NEOPIXEL_PIN 0
#define NUM_NEOPIXELS 1
#define NEOPIXEL_INDEX 0

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define DISPLAY_ADDRESS 0x3C
#define DISPLAY_RESET_PIN -1

class HCI{
public:
    HCI();
    void begin();
    bool buttonRead();
    void buzzSound(int var = 1, int freq = 1000, int ms = 1);
    void neoColor(int r, int g, int b);
    void showDebug(const char* currentState, const char* previousState,
                   uint16_t front, uint16_t left, uint16_t right,
                   float yaw, float targetYaw);
    bool isDisplayReady() const;

private:
    Adafruit_NeoPixel pixels;
    Adafruit_SSD1306 display;
    int soundVariations[5] = {2000,700,2000,2500,500};
    bool displayReady = false;
};
#endif
