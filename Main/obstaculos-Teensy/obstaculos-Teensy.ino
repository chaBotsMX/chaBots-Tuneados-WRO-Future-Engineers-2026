#include <Adafruit_NeoPixel.h>
#include "IMU.h"
#include <Servo.h>

#define BUTTON      10
#define BUZZER      11
#define SERVOMOTOR  12
#define INA         3
#define INB         1
#define PWM         2

int butState = 0;

Servo servo;
IMU imu;
Adafruit_NeoPixel pixels(1, 22, NEO_GRB + NEO_KHZ800);

// UARTs usados:
// Serial4 = IMU
// Serial5 = TOF
// Serial3 = OpenMV

// Servo
const int servoCenter = 90;
const int servoMin = 60;
const int servoMax = 120;

int lastServoCmd = servoCenter;
int recoveryServoCmd = servoCenter;

// IMU
int setPointIMU = 0;
int errorIMU = 0;

float kpIMU = 0.95f;
float kdIMU = 0.20f;
float kpTurn = 1.50f;

float lastErrorIMU = 0.0f;
elapsedMillis lastReadIMU;

// Motor
const int normalPWM = 120;
const int turnPWM = 85;
const int reversePWM = 100;

// TOF
const uint16_t TOF_MAX_VALID_MM = 3000;
const int frontTurnThreshold = 600;
const int turnFinishError = 3;

uint16_t front = 1500;
uint16_t left = 0;
uint16_t right = 0;

// Giro / estados
bool finish = false;
bool turning = false;
bool turnArmed = true;
bool reversing = false;

elapsedMillis reverseTimer;
const uint32_t REVERSE_TIME_MS = 3000;

int turnCount = 0;
const int MAX_TURNS = 12;

// Detección automática de sentido
bool directionChosen = false;
bool choosingDirection = false;
elapsedMillis directionChoiceTimer;

const uint32_t DIRECTION_CHOICE_WAIT_MS = 1000;

// +1 = antihorario / izquierda / +90
// -1 = horario / derecha / -90
int turnDirection = 1;

// Confirmación de pared real
elapsedMillis noPillarTimer;
elapsedMillis wallConfirmTimer;

const uint32_t NO_PILLAR_BEFORE_TURN_MS = 350;
const uint32_t WALL_CONFIRM_MS = 120;

// Recuperación corta
bool recovering = false;
bool retryTurnAfterRecovery = false;
elapsedMillis recoveryTimer;

const uint32_t RECOVERY_REVERSE_TIME_MS = 1500; // corregido de 15h00
const uint16_t PILLAR_DANGER_FRONT_MM = 50;

const int RECOVERY_TURN_FINISH_ERROR = turnFinishError + 2;

elapsedMillis turnTimer;
const uint32_t TURN_FAIL_TIME_MS = 1400;

// OpenMV
struct OpenMVBlob {
    uint16_t x = 0;
    uint16_t y = 0;
    uint8_t colorId = 0;
    bool valid = false;
};

OpenMVBlob camBlob;

const int CAM_WIDTH = 320;
const int CAM_CENTER_X = 160;

const float CAM_HALF_FOV_DEG = 30.0f;

const bool CAM_INVERT_X = false;

const uint8_t COLOR_RED = 1;
const uint8_t COLOR_GREEN = 2;

const int PASS_OFFSET_DEG = 30;
const int VISION_DEAD_ZONE_DEG = 20;

const int MAX_VISION_CORRECTION_DEG = 60;
const int CORR_KP = 4;

elapsedMillis lastCamPacket;
const uint32_t CAM_TIMEOUT_MS = 300;

int wrap180(int ang) {
    while (ang > 180) ang -= 360;
    while (ang <= -180) ang += 360;
    return ang;
}

int angleErrorDeg(int target, int current) {
    return wrap180(target - current);
}

void writeServoCmd(int cmd) {
    cmd = constrain(cmd, servoMin, servoMax);
    lastServoCmd = cmd;
    servo.write(cmd);
}

int getInvertedServoCmd() {
    int inverted = (servoCenter * 2) - lastServoCmd;
    return constrain(inverted, servoMin, servoMax);
}

void motorForwardPWM(int pwm) {
    digitalWrite(INA, LOW);
    digitalWrite(INB, HIGH);
    analogWrite(PWM, constrain(pwm, 0, 255));
}

void motorReversePWM(int pwm) {
    digitalWrite(INA, HIGH);
    digitalWrite(INB, LOW);
    analogWrite(PWM, constrain(pwm, 0, 255));
}

void motorStop() {
    digitalWrite(INA, LOW);
    digitalWrite(INB, LOW);
    analogWrite(PWM, 0);
}

bool readTOFPacket(HardwareSerial &port, uint16_t &front, uint16_t &left, uint16_t &right) {
    static uint8_t state = 0;
    static uint8_t buffer[6];
    static uint8_t index = 0;

    while (port.available()) {
        uint8_t c = port.read();

        switch (state) {
            case 0:
                if (c == 0xAA) {
                    index = 0;
                    state = 1;
                }
                break;

            case 1:
                buffer[index++] = c;

                if (index >= 6) {
                    uint16_t newFront = (uint16_t)buffer[0] | ((uint16_t)buffer[1] << 8);
                    uint16_t newLeft  = (uint16_t)buffer[2] | ((uint16_t)buffer[3] << 8);
                    uint16_t newRight = (uint16_t)buffer[4] | ((uint16_t)buffer[5] << 8);

                    if (newFront <= TOF_MAX_VALID_MM) front = newFront;
                    if (newLeft  <= TOF_MAX_VALID_MM) left = newLeft;
                    if (newRight <= TOF_MAX_VALID_MM) right = newRight;

                    Serial.println(newLeft);

                    state = 0;
                    return true;
                }
                break;
        }
    }

    return false;
}

bool readOpenMVPacket(HardwareSerial &port, OpenMVBlob &blob) {
    static uint8_t state = 0;
    static uint8_t buffer[5];
    static uint8_t index = 0;

    while (port.available()) {
        uint8_t c = port.read();

        switch (state) {
            case 0:
                if (c == 0xAA) {
                    index = 0;
                    state = 1;
                }
                break;

            case 1:
                buffer[index++] = c;

                if (index >= 5) {
                    state = 2;
                }
                break;

            case 2:
                if (c == 0x55) {
                    blob.colorId = buffer[0];
                    blob.x = (uint16_t)buffer[1] | ((uint16_t)buffer[2] << 8);
                    blob.y = (uint16_t)buffer[3] | ((uint16_t)buffer[4] << 8);
                    blob.valid = true;

                    state = 0;
                    return true;
                } else {
                    state = 0;
                    index = 0;

                    if (c == 0xAA) {
                        state = 1;
                    }
                }
                break;
        }
    }

    return false;
}

float getBlobAngleDeg(const OpenMVBlob &blob) {
    int dx = (int)blob.x - CAM_CENTER_X;

    float angle = ((float)dx * CAM_HALF_FOV_DEG) / (float)CAM_CENTER_X;

    if (CAM_INVERT_X) {
        angle = -angle;
    }

    return constrain(angle, -CAM_HALF_FOV_DEG, CAM_HALF_FOV_DEG);
}

bool hasFreshBlob() {
    return camBlob.valid && lastCamPacket <= CAM_TIMEOUT_MS;
}

// NO CAMBIÉ ESTA LÓGICA DE EVASIÓN
int getVisionTargetIMU() {
    if (!hasFreshBlob()) {
        return setPointIMU;
    }

    float blobAngle = getBlobAngleDeg(camBlob);
    float correction = 0.0f;

    if (camBlob.colorId == COLOR_GREEN) {
        if (blobAngle > -VISION_DEAD_ZONE_DEG) {
            correction = blobAngle + PASS_OFFSET_DEG;
        }
    }
    else if (camBlob.colorId == COLOR_RED) {
        if (blobAngle < VISION_DEAD_ZONE_DEG) {
            correction = blobAngle - PASS_OFFSET_DEG;
        }
    }

    correction = constrain(correction * CORR_KP, -MAX_VISION_CORRECTION_DEG, MAX_VISION_CORRECTION_DEG);

    return wrap180(setPointIMU + (int)correction);
}

void updateHeadingSteering(int yaw, int targetIMU) {
    errorIMU = angleErrorDeg(targetIMU, yaw);

    float dtIMU = max(1.0f, (float)lastReadIMU);
    float dErrIMU = (errorIMU - lastErrorIMU) / dtIMU;

    float imuTerm = kpIMU * errorIMU + kdIMU * dErrIMU;

    lastErrorIMU = errorIMU;
    lastReadIMU = 0;

    int servoCmd = constrain((int)(servoCenter + imuTerm), servoMin, servoMax);

    if (left < 100) {
        writeServoCmd(60);
    }
    else if (right < 100) {
        writeServoCmd(120);
    }
    else {
        writeServoCmd(servoCmd);
    }
}

void resetWallConfirmTimers() {
    wallConfirmTimer = 0;
    noPillarTimer = 0;
}

bool pillarTooClose() {
    return hasFreshBlob() && front > 0 && front < PILLAR_DANGER_FRONT_MM;
}

void startReverse() {
    reversing = true;
    reverseTimer = 0;
    turnArmed = false;
    writeServoCmd(servoCenter);

    resetWallConfirmTimers();
}

void startRecoveryReverse(bool retryTurn) {
    recovering = true;
    retryTurnAfterRecovery = retryTurn;
    recoveryTimer = 0;

    turning = false;
    reversing = false;
    turnArmed = false;

    resetWallConfirmTimers();

    recoveryServoCmd = getInvertedServoCmd();

    writeServoCmd(recoveryServoCmd);
    tone(BUZZER, 900, 80);
}

bool recoveryTurnAlreadyAligned(int imuRead) {
    if (!retryTurnAfterRecovery) {
        return false;
    }

    int turnError = abs(angleErrorDeg(setPointIMU, imuRead));
    return turnError <= RECOVERY_TURN_FINISH_ERROR;
}

void finishWallTurnFromRecovery() {
    recovering = false;
    retryTurnAfterRecovery = false;
    turning = false;

    motorStop();
    resetWallConfirmTimers();

    startReverse();

    if (MAX_TURNS > 0 && turnCount >= MAX_TURNS) {
        finish = true;
    }
}

void finishRecoveryAndRetrySameTurn() {
    recovering = false;
    motorStop();

    resetWallConfirmTimers();

    retryTurnAfterRecovery = false;
    turning = true;
    turnArmed = false;
    turnTimer = 0;
}

void finishPillarRecovery() {
    recovering = false;
    motorStop();

    resetWallConfirmTimers();

    retryTurnAfterRecovery = false;
    turnArmed = true;
}

void updateRecoveryReverse(int imuRead) {
    writeServoCmd(recoveryServoCmd);
    motorReversePWM(reversePWM);

    resetWallConfirmTimers();

    if (recoveryTurnAlreadyAligned(imuRead)) {
        finishWallTurnFromRecovery();
        return;
    }

    if (recoveryTimer >= RECOVERY_REVERSE_TIME_MS) {
        if (retryTurnAfterRecovery) {
            finishRecoveryAndRetrySameTurn();
        } else {
            finishPillarRecovery();
        }
    }
}

void startTurnByDirection() {
    setPointIMU = wrap180(setPointIMU + (90 * turnDirection));
    turning = true;
    turnArmed = false;
    turnTimer = 0;

    resetWallConfirmTimers();
}

void startDirectionChoice() {
    choosingDirection = true;
    directionChoiceTimer = 0;
    turnArmed = false;

    motorStop();
    writeServoCmd(servoCenter);

    resetWallConfirmTimers();

    tone(BUZZER, 1100, 100);
}

void finishDirectionChoiceAndStartTurn() {
    choosingDirection = false;
    directionChosen = true;

    if (left >= right) {
        turnDirection = 1;   // antihorario, vueltas a la izquierda
        tone(BUZZER, 1500, 100);
    } else {
        turnDirection = -1;  // horario, vueltas a la derecha
        tone(BUZZER, 700, 100);
    }

    startTurnByDirection();
    turnCount++;
}

void updateDirectionChoice() {
    motorStop();
    writeServoCmd(servoCenter);

    resetWallConfirmTimers();

    if (directionChoiceTimer >= DIRECTION_CHOICE_WAIT_MS) {
        finishDirectionChoiceAndStartTurn();
    }
}

void updateTurn(int yaw) {
    int turnError = angleErrorDeg(setPointIMU, yaw);

    int servoCmd = constrain((int)(servoCenter + kpTurn * turnError), servoMin, servoMax);
    writeServoCmd(servoCmd);

    if (abs(turnError) <= turnFinishError) {
        turning = false;
    }
}

bool turnLooksStuck(int imuRead) {
    if (!turning) {
        return false;
    }

    int turnError = abs(angleErrorDeg(setPointIMU, imuRead));

    if (turnError <= turnFinishError + 2) {
        return false;
    }

    if (front < frontTurnThreshold && turnTimer >= TURN_FAIL_TIME_MS) {
        return true;
    }

    return false;
}

void updateTurnCondition(int imuRead) {
    int baseHeadingError = angleErrorDeg(setPointIMU, imuRead);

    bool pillarVisible = hasFreshBlob();

    if (pillarVisible) {
        resetWallConfirmTimers();
        return;
    }

    if (front >= frontTurnThreshold) {
        wallConfirmTimer = 0;
        return;
    }

    bool noPillarConfirmed = noPillarTimer >= NO_PILLAR_BEFORE_TURN_MS;
    bool wallConfirmed = wallConfirmTimer >= WALL_CONFIRM_MS;

    if (turnArmed &&
        wallConfirmed &&
        noPillarConfirmed &&
        abs(baseHeadingError) < 10) {

        if (!directionChosen) {
            startDirectionChoice();
        } else {
            startTurnByDirection();
            turnCount++;
            tone(BUZZER, 1500, 100);
        }
    }
}

void setup() {
    pinMode(INA, OUTPUT);
    pinMode(INB, OUTPUT);
    pinMode(PWM, OUTPUT);

    pinMode(BUTTON, INPUT);
    pinMode(BUZZER, OUTPUT);

    analogWriteFrequency(PWM, 20000);

    Serial.begin(115200);

    Serial4.begin(115200);
    Serial5.begin(2000000);
    Serial3.begin(115200);

    pixels.begin();
    pixels.clear();
    pixels.show();

    servo.attach(SERVOMOTOR);
    writeServoCmd(servoCenter);

    imu.begin(Serial4);

    motorStop();

    delay(1000);
}

void loop() {
    while (butState == 0) {
        motorStop();
        writeServoCmd(servoCenter);

        if (digitalRead(BUTTON) == 1) {
            butState = 1;
            tone(BUZZER, 700, 500);
            delay(300);

            resetWallConfirmTimers();
        }

        bool newTofStart = readTOFPacket(Serial5, front, left, right);

        if (readOpenMVPacket(Serial3, camBlob)) {
            lastCamPacket = 0;
        }

        if (newTofStart) {
            pixels.setPixelColor(0, pixels.Color(50, 50, 50));
        } else {
            pixels.setPixelColor(0, pixels.Color(0, 0, 0));
        }

        pixels.show();
    }

    imu.update();
    int imuRead = wrap180(imu.getYaw());

    bool newTof = readTOFPacket(Serial5, front, left, right);

    if (readOpenMVPacket(Serial3, camBlob)) {
        lastCamPacket = 0;

        float blobAngle = getBlobAngleDeg(camBlob);
        int visionTarget = getVisionTargetIMU();

        /*
        Serial.print("Color ID: ");
        Serial.print(camBlob.colorId);

        Serial.print(" X: ");
        Serial.print(camBlob.x);

        Serial.print(" Y: ");
        Serial.print(camBlob.y);

        Serial.print(" Angle: ");
        Serial.print(blobAngle);

        Serial.print(" TargetIMU: ");
        Serial.print(visionTarget);

        Serial.print(" Front: ");
        Serial.print(front);

        Serial.print(" Left: ");
        Serial.print(left);

        Serial.print(" Right: ");
        Serial.print(right);

        Serial.print(" DirectionChosen: ");
        Serial.print(directionChosen);

        Serial.print(" TurnDirection: ");
        Serial.println(turnDirection);
        */
    }

    if (finish) {
        motorStop();
        writeServoCmd(servoCenter);
        pixels.setPixelColor(0, pixels.Color(50, 0, 0));
        pixels.show();
        return;
    }

    if (choosingDirection) {
        updateDirectionChoice();
        pixels.setPixelColor(0, pixels.Color(20, 20, 50));
        pixels.show();
        return;
    }

    if (recovering) {
        updateRecoveryReverse(imuRead);
        pixels.setPixelColor(0, pixels.Color(50, 20, 0));
        pixels.show();
        return;
    }

    if (!turning && !reversing && pillarTooClose()) {
        startRecoveryReverse(false);
        pixels.setPixelColor(0, pixels.Color(50, 20, 0));
        pixels.show();
        return;
    }

    if (reversing) {
        writeServoCmd(servoCenter);
        motorReversePWM(reversePWM);

        resetWallConfirmTimers();

        if (reverseTimer >= REVERSE_TIME_MS) {
            reversing = false;
            motorStop();

            if (front > frontTurnThreshold + 100) {
                turnArmed = true;
            }
        }

        return;
    }

    if (!turning && front > frontTurnThreshold + 100) {
        turnArmed = true;
    }

    if (turning) {
        motorForwardPWM(turnPWM);

        resetWallConfirmTimers();

        bool wasTurning = turning;
        updateTurn(imuRead);

        if (turnLooksStuck(imuRead)) {
            startRecoveryReverse(true);
            pixels.setPixelColor(0, pixels.Color(50, 20, 0));
            pixels.show();
            return;
        }

        if (wasTurning && !turning) {
            startReverse();

            if (MAX_TURNS > 0 && turnCount >= MAX_TURNS) {
                finish = true;
            }
        }

        return;
    }

    int visionTarget = getVisionTargetIMU();
    updateHeadingSteering(imuRead, visionTarget);
    motorForwardPWM(60);

    updateTurnCondition(imuRead);

    if (newTof) {
        pixels.setPixelColor(0, pixels.Color(0, 0, 50));
    }
    else if (hasFreshBlob()) {
        if (camBlob.colorId == COLOR_GREEN) {
            pixels.setPixelColor(0, pixels.Color(0, 50, 0));
        } else if (camBlob.colorId == COLOR_RED) {
            pixels.setPixelColor(0, pixels.Color(50, 0, 0));
        } else {
            pixels.setPixelColor(0, pixels.Color(30, 30, 0));
        }
    }
    else {
        pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    }

    pixels.show();
}