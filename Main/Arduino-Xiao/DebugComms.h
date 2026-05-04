/**
 * @file debugComms.h
 * @brief Header file for managing debug communications over wifi.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 03/05/26
 */



#ifndef DEBUGCOMMS_H
#define DEBUGCOMMS_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "DebugLog.h"

static inline IPAddress getBroadcastIP() {
  IPAddress ip = WiFi.localIP();
  IPAddress subnet = WiFi.subnetMask();

  IPAddress broadcast;
  for (int i = 0; i < 4; i++) {
    broadcast[i] = ip[i] | ~subnet[i];
  }

  return broadcast;
}

class DebugComms {
public:
    
    DebugComms(bool _wifiMode, uint8_t _txRate, const char* _existingSSID, const char* _existingPass);
    void begin();
    void sendDebugData(const uint8_t data[], size_t length);
    void setIp();
private:
    elapsedMillis debugCommsTimer; 
    WiFiUDP udp;
    IPAddress sendingIP;
    // Expected IP address of the laptop.
    IPAddress laptopIP = IPAddress(192, 168, 4, 2);

    // credentials for connecting to an existing wifi network.
    const char* EXISTING_WIFI_SSID = "";
    const char* EXISTING_WIFI_PASS = "";

    // if connection is set to AP mode, these credentials will be used for the AP.
    const char* AP_WIFI_SSID = "BOBOT-WRO-CHABOTS";
    const char* AP_WIFI_PASS = "dreamRobots";

    // port for sending debug data.
    const int UDP_PORT = 5005;

    uint8_t TX_RATE_HZ = 10; // in Hz, how often to send debug data.
    uint8_t TRANSMITTING_RATE = 1000 / TX_RATE_HZ; // in ms, how often to send debug data.
    // This is the correct way of doing this? or should i make a function that calculates TRANSMITTING_RATE once?
    // i dont know if the calculation will be done every time TRANSMITTING_RATE is used, or if it will be calculated 
    // once and then stored as a constant. 
    

    bool wifiMode = 1; // 0 for existing wifi, 1 for AP mode.
};

#endif