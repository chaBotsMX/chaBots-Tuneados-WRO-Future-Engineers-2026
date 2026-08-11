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

#define BUTTON 10
#define BUZZER 11
#define NEOPIXEL_PIN 22
#define NUM_NEOPIXELS 1
#define LED_PIN 13

class HCI{
public:
    HCI();
    void begin();
    bool buttonRead();
    void buzzSound(int var = 1, int freq = 1000, int ms = 500);
    void neoColor(int r, int g, int b);

private:
Adafruit_NeoPixel pixels;
int soundVariations[5] = {2000,700,2000,2500,500};


};
#endif