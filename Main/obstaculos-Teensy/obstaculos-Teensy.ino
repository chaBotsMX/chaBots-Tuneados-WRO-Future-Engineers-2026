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

// IMU
int setPointIMU = 0;   // heading base deseado
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
const int MAX_TURNS = 12;   // pon 0 si no quieres límite de vueltas

// Confirmación de pared real.
// Esto evita que el robot confunda un pilar con la pared.
elapsedMillis noPillarTimer;
elapsedMillis wallConfirmTimer;

const uint32_t NO_PILLAR_BEFORE_TURN_MS = 350;
const uint32_t WALL_CONFIRM_MS = 120;

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

// Asumimos que la cámara ve aproximadamente de -30 a +30 grados.
// Si tu lente/ROI cambia, ajusta este valor.
const float CAM_HALF_FOV_DEG = 30.0f;

// Si al probar corrige al lado contrario, cambia esto a true.
const bool CAM_INVERT_X = false;

// Según tu código OpenMV:
// thresholds[0] = rojo  -> ID 1
// thresholds[1] = verde -> ID 2
const uint8_t COLOR_RED = 1;
const uint8_t COLOR_GREEN = 2;

// Offset de evasión
const int PASS_OFFSET_DEG = 30;

// Zona muerta para evitar correcciones por ruido cerca del centro
const int VISION_DEAD_ZONE_DEG = 20;

// Límite máximo de corrección
const int MAX_VISION_CORRECTION_DEG = 60;
const int CORR_KP = 4;

// Timeout para no usar un blob viejo
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
                    if (newLeft  <= TOF_MAX_VALID_MM) left  = newLeft;
                    if (newRight <= TOF_MAX_VALID_MM) right = newRight;

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
                // color_id, xL, xH, yL, yH
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
        // Verde se pasa por la izquierda.
        // Según tu lógica:
        // Verde en -30 está mal -> aplicar offset -30 -> objetivo -60.
        // Verde en +30 ya está bien -> no corregir.
        if (blobAngle > -VISION_DEAD_ZONE_DEG) {
            correction = blobAngle + PASS_OFFSET_DEG;
        }
    }
    else if (camBlob.colorId == COLOR_RED) {
        // Rojo se pasa por la derecha.
        // Rojo en +30 está mal -> aplicar offset +30 -> objetivo +60.
        // Rojo en -30 ya está bien -> no corregir.
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
    servo.write(servoCmd);
}

void startTurnLeft() {
    setPointIMU = wrap180(setPointIMU + 90);
    turning = true;
    turnArmed = false;

    wallConfirmTimer = 0;
    noPillarTimer = 0;
}

void updateTurn(int yaw) {
    int turnError = angleErrorDeg(setPointIMU, yaw);

    int servoCmd = constrain((int)(servoCenter + kpTurn * turnError), servoMin, servoMax);
    servo.write(servoCmd);

    if (abs(turnError) <= turnFinishError) {
        turning = false;
    }
}

void startReverse() {
    reversing = true;
    reverseTimer = 0;
    turnArmed = false;
    servo.write(servoCenter);

    wallConfirmTimer = 0;
    noPillarTimer = 0;
}

void updateTurnCondition(int imuRead) {
    int baseHeadingError = angleErrorDeg(setPointIMU, imuRead);

    bool pillarVisible = hasFreshBlob();

    // Si la OpenMV ve un pilar, no se puede confirmar pared.
    if (pillarVisible) {
        noPillarTimer = 0;
        wallConfirmTimer = 0;
        return;
    }

    // Si el frente ya no está cerca, se cancela la confirmación de pared.
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

        startTurnLeft();
        turnCount++;
        tone(BUZZER, 1500, 100);
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

    Serial4.begin(115200);     // IMU
    Serial5.begin(2000000);    // TOF
    Serial3.begin(115200);     // OpenMV

    pixels.begin();
    pixels.clear();
    pixels.show();

    servo.attach(SERVOMOTOR);
    servo.write(servoCenter);

    imu.begin(Serial4);

    motorStop();

    delay(1000);
}

void loop() {
    while (butState == 0) {
        motorStop();
        servo.write(servoCenter);

        if (digitalRead(BUTTON) == 1) {
            butState = 1;
            tone(BUZZER, 700, 500);
            delay(300);

            wallConfirmTimer = 0;
            noPillarTimer = 0;
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

        Serial.print(" Pillar: ");
        Serial.print(hasFreshBlob());

        Serial.print(" WallConfirm: ");
        Serial.print((uint32_t)wallConfirmTimer);

        Serial.print(" NoPillar: ");
        Serial.println((uint32_t)noPillarTimer);
    }

    if (finish) {
        motorStop();
        servo.write(servoCenter);
        pixels.setPixelColor(0, pixels.Color(50, 0, 0));
        pixels.show();
        return;
    }

    // Retroceso después de terminar un giro.
    if (reversing) {
        servo.write(servoCenter);
        motorReversePWM(reversePWM);

        wallConfirmTimer = 0;
        noPillarTimer = 0;

        if (reverseTimer >= REVERSE_TIME_MS) {
            reversing = false;
            motorStop();

            // No se rearma inmediatamente si todavía está viendo pared.
            if (front > frontTurnThreshold + 100) {
                turnArmed = true;
            }
        }

        return;
    }

    // Rearmar giro cuando el frente esté despejado.
    if (!turning && front > frontTurnThreshold + 100) {
        turnArmed = true;
    }

    // Giro de 90 grados con IMU.
    if (turning) {
        motorForwardPWM(turnPWM);

        wallConfirmTimer = 0;
        noPillarTimer = 0;

        bool wasTurning = turning;
        updateTurn(imuRead);

        if (wasTurning && !turning) {
            startReverse();

            if (MAX_TURNS > 0 && turnCount >= MAX_TURNS) {
                finish = true;
            }
        }

        return;
    }

    // Movimiento normal con evasión por cámara.
    int visionTarget = getVisionTargetIMU();
    updateHeadingSteering(imuRead, visionTarget);
    motorForwardPWM(60);

    // Nueva condición de giro:
    // Solo gira si front detecta algo cerca,
    // pero OpenMV NO está viendo un pilar por un tiempo.
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