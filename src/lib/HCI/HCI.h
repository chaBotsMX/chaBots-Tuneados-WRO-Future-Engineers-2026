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
    // var uses indices 1..5; with var <= 0, freq is played for ms.
    void buzzSound(int var = 1, int freq = 1000, int ms = 100);
    void neoColor(int r, int g, int b);
    // Optional: omit this call to leave the display and its I2C bus unused.
    void beginDebugDisplay();
    // Boot only, with the motor stopped: finish one frame before starting a sensor.
    void showStartupStatus(const char* state);
    // Print the saved OLED diagnosis after USB Serial has been initialized.
    void printDebugDisplayStatus() const;
    void showDebug(const char* currentState,
                   uint16_t front, uint16_t left, uint16_t right,
                   float yaw, float targetYaw, bool cameraUartOk = false);
    bool isDisplayReady() const;

private:
    static constexpr uint16_t DISPLAY_BUFFER_SIZE = DISPLAY_WIDTH * DISPLAY_HEIGHT / 8;
    static constexpr uint32_t DISPLAY_REFRESH_MS = 100;
    Adafruit_NeoPixel pixels;
    Adafruit_SSD1306 display;
    int soundVariations[5] = {2000,700,2000,2500,500};
    bool displayReady = false;
    uint8_t displayAddress = DISPLAY_ADDRESS;
    uint8_t displayI2cError = 0;
    const char* displayStatus = "DESACTIVADA";
    uint32_t lastDisplayRefreshMs = 0;
    uint16_t displayOffset = DISPLAY_BUFFER_SIZE;

    bool writeDisplayPacket(uint8_t control, const uint8_t* bytes, size_t count);
    void printDistance(const char* label, uint16_t distance, int16_t y);
};
#endif
