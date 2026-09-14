/**
 * @file HCI.cpp
 * @brief library to manage user interaction with the robot.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 11/08/26
 */

#include "HCI.h"
#include "ControlValues.h"
#include <math.h>


 HCI::HCI():
        pixels(NUM_NEOPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800),
        display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, DISPLAY_RESET_PIN,
                400000, 400000)
 {}

void HCI::begin(){
    pinMode(BUTTON,INPUT);
    pinMode(BUZZER,OUTPUT);

    pixels.begin();
    pixels.setPixelColor(NEOPIXEL_INDEX, pixels.Color(255, 0, 0)); // Set the LED color to red
    pixels.show(); // Update the LED to show the color
    buzzSound(1); // Audible confirmation that setup reached the indicators.
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

void HCI::printDebugDisplayStatus() const {
    Serial.print("[OLED] ");
    Serial.print(displayStatus);
    Serial.print("; direccion=0x");
    Serial.print(displayAddress, HEX);
    Serial.print("; error I2C=");
    Serial.println(displayI2cError);
}

bool HCI::writeDisplayPacket(uint8_t control, const uint8_t* bytes, size_t count) {
    // SDA/SCL belong to Wire now, not GPIO: digitalRead() is not a valid
    // communication test. Only the I2C controller's result decides success.
    Wire.beginTransmission(displayAddress);
    if (Wire.write(control) != 1 || Wire.write(bytes, count) != count) {
        displayI2cError = 1;
    } else {
        displayI2cError = Wire.endTransmission();
    }
    if (displayI2cError != 0) {
        // Stop at the first failed refresh packet. No automatic runtime retries.
        displayReady = false;
        displayStatus = "ERROR DE ENVIO";
        return false;
    }
    return true;
}

void HCI::beginDebugDisplay() {
    displayReady = false;
    displayStatus = "INICIANDO";

    Wire.begin();
    delay(100);
    Wire.setClock(400000);
    delay(100);

    // Check the two SSD1306 addresses, once at boot. Try the alternative only
    // on NACK (2); a timeout or bus fault must not trigger more transactions.
    const uint8_t addresses[] = {DISPLAY_ADDRESS, DISPLAY_ADDRESS ^ 0x01};
    for (uint8_t address : addresses) {
        displayAddress = address;
        Wire.beginTransmission(displayAddress);
        displayI2cError = Wire.endTransmission();
        if (displayI2cError != 2) {
            break;
        }
    }
    if (displayI2cError != 0) {
        displayStatus = displayI2cError == 2 ? "SIN RESPUESTA EN 0x3C/0x3D" : "ERROR DE BUS";
        return;
    }

    // Restore the library initialization used by the previously working code.
    // This runs once, before motor control. begin() alone does NOT prove an ACK.
    if (!display.begin(SSD1306_SWITCHCAPVCC, displayAddress, false, false)) {
        displayStatus = "SIN MEMORIA";
        return;
    }
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setTextWrap(false);
    displayOffset = DISPLAY_BUFFER_SIZE;
    lastDisplayRefreshMs = millis() - DISPLAY_REFRESH_MS;
    displayReady = true;
    displayStatus = "I2C OK";
    showStartupStatus("OLED OK"); // Send a visible frame before any sensor can block.
}

void HCI::showStartupStatus(const char* state) {
    if (!displayReady) {
        return;
    }
    lastDisplayRefreshMs = millis() - DISPLAY_REFRESH_MS;
    displayOffset = DISPLAY_BUFFER_SIZE;
    do {
        showDebug(state, MAX_VALID_DISTANCE, MAX_VALID_DISTANCE,
                  MAX_VALID_DISTANCE, NAN, NAN);
    } while (displayReady && displayOffset < DISPLAY_BUFFER_SIZE);
}

void HCI::printDistance(const char* label, uint16_t distance, int16_t y) {
    display.setCursor(0, y);
    display.print(label);
    if (distance >= MAX_VALID_DISTANCE) {
        display.print("INVALIDA");
    } else {
        display.print(distance);
        display.print("mm");
    }
}

void HCI::showDebug(const char* currentState,
                    uint16_t front, uint16_t left, uint16_t right,
                    float yaw, float targetYaw, bool cameraUartOk){
    if (!displayReady) {
        return;
    }

    if (displayOffset < DISPLAY_BUFFER_SIZE) {
        // At most 31 image bytes per loop (~0.75 ms at 400 kHz), so sensor
        // updates and motor control run between packets. Keep this frame frozen.
        const size_t count = min(size_t(31), size_t(DISPLAY_BUFFER_SIZE - displayOffset));
        if (writeDisplayPacket(0x40, display.getBuffer() + displayOffset, count)) {
            displayOffset += count;
        }
        return;
    }

    const uint32_t now = millis();
    if (static_cast<uint32_t>(now - lastDisplayRefreshMs) < DISPLAY_REFRESH_MS) {
        return;
    }
    lastDisplayRefreshMs = now;

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("EST:");
    display.print(currentState);
    display.drawFastHLine(0, 10, DISPLAY_WIDTH, SSD1306_WHITE);
    printDistance("F:", front, 16);
    printDistance("I:", left, 26);
    printDistance("D:", right, 36);
    display.setCursor(0, 46);
    display.print("OBJ:");
    if (isfinite(targetYaw)) {
        display.print(targetYaw, 0);
    } else {
        display.print("--");
    }

    constexpr int16_t cx = 104, cy = 32, radius = 19;
    display.drawCircle(cx, cy, radius, SSD1306_WHITE);
    if (isfinite(yaw) && isfinite(targetYaw)) {
        // Heading relative to the robot: straight is up, positive yaw is left.
        const float angle = fmodf(targetYaw - yaw, 360.0f) * DEG_TO_RAD;
        const int16_t x = cx - lroundf(sinf(angle) * (radius - 2));
        const int16_t y = cy - lroundf(cosf(angle) * (radius - 2));
        display.drawLine(cx, cy, x, y, SSD1306_WHITE);
        display.fillCircle(x, y, 2, SSD1306_WHITE);
        display.fillCircle(cx, cy, 1, SSD1306_WHITE);
    } else {
        display.setCursor(cx - 3, cy - 4);
        display.print("?");
    }

    display.setCursor(0, 56);
    display.print(cameraUartOk ? "CAM UART: OK" : "CAM UART: ERROR");

    static const uint8_t window[] = {
        SSD1306_PAGEADDR, 0, DISPLAY_HEIGHT / 8 - 1,
        SSD1306_COLUMNADDR, 0, DISPLAY_WIDTH - 1
    };
    if (writeDisplayPacket(0x00, window, sizeof(window))) {
        displayOffset = 0;
    }
}
