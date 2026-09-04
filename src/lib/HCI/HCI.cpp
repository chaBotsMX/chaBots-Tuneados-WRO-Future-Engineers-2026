/**
 * @file HCI.cpp
 * @brief library to manage user interaction with the robot.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 11/08/26
 */

 #include "HCI.h"


 HCI::HCI():
        pixels(NUM_NEOPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800),
        display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, DISPLAY_RESET_PIN)
 {}

void HCI::begin(){
    pinMode(BUTTON,INPUT);
    pinMode(BUZZER,OUTPUT);

    pixels.begin();
    pixels.setPixelColor(NEOPIXEL_INDEX, pixels.Color(255, 0, 0)); // Set the LED color to red
    pixels.show(); // Update the LED to show the color

    Wire.begin();
    Wire.setClock(400000); // 400kHz Fast Mode
     
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

bool HCI::isDisplayReady() const {
    return displayReady;
}

void HCI::showDebug(const char* currentState, const char* previousState,
                    uint16_t front, uint16_t left, uint16_t right,
                    float yaw, float targetYaw){
    if (!displayReady) {
        return;
    }

    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("CURRENT STATE:");
    display.println(currentState);
    display.println("PREVIOUS STATE:");
    display.println(previousState);
    display.print("F:");
    display.print(front);
    display.print(" L:");
    display.println(left);
    display.print("R:");
    display.println(right);
    display.print("YAW:");
    display.println(yaw, 1);
    display.print("TARGET:");
    display.println(targetYaw, 1);
    display.display();
}
