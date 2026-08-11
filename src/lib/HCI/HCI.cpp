/**
 * @file HCI.cpp
 * @brief library to manage user interaction with the robot.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 11/08/26
 */

 #include "HCI.h"

 Adafruit_NeoPixel pixels(NUM_NEOPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
 
 HCI::HCI(){}

void HCI::begin(){
    pinMode(BUTTON,INPUT);
    pinMode(BUZZER,OUTPUT);
    pinMode(LED_PIN,OUTPUT);

    pixels.begin();
    pixels.setPixelColor(0, pixels.Color(255, 0, 0)); // Set the LED color to red
    pixels.show(); // Update the LED to show the color

}

bool HCI::buttonRead(){
    return digitalRead(BUTTON);
}

void HCI::buzzSound(int var,int freq, int ms){    
    if(var > 0){
        tone(BUZZER,soundVariations[var-1],ms);
    }
    else{
        tone(BUZZER,freq,ms);
    }
}

void HCI::neoColor(int r, int g, int b){
    pixels.setPixelColor(0, pixels.Color(r,g,b)); // Set the LED color to green
    pixels.show(); // Update the LED to show the color
}

