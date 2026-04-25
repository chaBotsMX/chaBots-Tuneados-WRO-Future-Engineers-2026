#include <Adafruit_NeoPixel.h>
#include "IMU.h"
#include <Servo.h>
#include <Encoder.h>

#define BUTTON      10
#define BUZZER      11
#define SERVOMOTOR  12
#define INA         3
#define INB         1
#define PWM         2
#define ENCA        9
#define ENCB        8

#define CPRPERSHAFTREVOLUTION 979.6128f
#define MAXRPS 8.3333f
#define DISTANCEPERREVOLUTION 13.037f   // cm
const float MM_PER_TICK = (DISTANCEPERREVOLUTION * 10.0f) / CPRPERSHAFTREVOLUTION;

int butState = 0;

Servo servo;
IMU imu;
Encoder motorEnc(ENCA, ENCB);
Adafruit_NeoPixel pixels(1, 22, NEO_GRB + NEO_KHZ800);

int setPointWall = 300;     // mm a la pared seguida
int setPointIMU  = 0;       // heading deseado en rango -180..180

const int servoCenter = 90;

// Ganancias
float kpWall = 0.25f;
float kpIMU  = 0.55f;
float kpTurn = 1.50f;

float kdWall = 0.15f;
float kdIMU  = 0.25f;

float lastErrorIMU  = 0.0f;
float lastErrorWall = 0.0f;

elapsedMillis lastReadWall;
elapsedMillis lastReadIMU;

// Límites servo
const int servoMin = 60;
const int servoMax = 120;

// Umbrales
const int frontTurnThreshold = 600;   // mm para comenzar giro
const int turnFinishError    = 10;    // grados tolerancia de fin de giro

// Validación pared
const int wallMinValid = 0;
const int wallMaxValid = 2000;
const int maxWallError = 300;

elapsedMillis finalTimer;
bool finalAdvanceStarted = false;

// Filtro de salto de pared.
// Antes de saber el sentido, si la pared seguida brinca mucho,
// se ignora la pared y se usa solo IMU.
const int WALL_JUMP_THRESHOLD_MM = 1000;
uint16_t lastWallDistForJump = 0;
bool lastWallDistValidForJump = false;

// Velocidades máximas
const int maxPWM = 130;
const int minPWM = 70;

// Filtro de TOF
const uint16_t TOF_MAX_VALID_MM = 3000;

// Distancias estimadas continuas
float estFront = 0.0f;
float estLeft  = 0.0f;
float estRight = 0.0f;
bool distInitialized = false;

uint16_t front = 1500, left = 0, right = 0;
bool finish = 0;
bool turning = false;
bool turnArmed = true;
int errorIMU = 0;
int outError = 0;
const int rP = 4;
int turnCount = 0;

// Selección automática de sentido
bool directionChosen = false;
bool choosingDirection = false;
elapsedMillis directionChoiceTimer;

const uint32_t DIRECTION_CHOICE_WAIT_MS = 1000;

// +1 = antihorario, gira izquierda, sigue pared derecha
// -1 = horario, gira derecha, sigue pared izquierda
int turnDirection = 1;

int wrap180(int ang) {
    while (ang > 180)  ang -= 360;
    while (ang <= -180) ang += 360;
    return ang;
}

int angleErrorDeg(int target, int current) {
    return wrap180(target - current);
}

int32_t getEncoderTicks() {
    return motorEnc.read();
}

void motorForwardPWM(int pwm) {
    digitalWrite(INA, LOW);
    digitalWrite(INB, HIGH);
    analogWrite(PWM, constrain(pwm, 0, 255));
}

void motorStop() {
    digitalWrite(INA, LOW);
    digitalWrite(INB, LOW);
    analogWrite(PWM, 0);
}

void resetWallJumpFilter() {
    lastWallDistForJump = 0;
    lastWallDistValidForJump = false;
}

void syncEstimateFromCurrentDistances() {
    estFront = (float)front;
    estLeft  = (float)left;
    estRight = (float)right;
    distInitialized = true;
}

void publishEstimatedDistances() {
    front = (uint16_t)constrain((int)estFront, 0, (int)TOF_MAX_VALID_MM);
    left  = (uint16_t)constrain((int)estLeft,  0, (int)TOF_MAX_VALID_MM);
    right = (uint16_t)constrain((int)estRight, 0, (int)TOF_MAX_VALID_MM);
}

void updateEstimatedDistances(int yaw) {
    static int32_t lastTicks = 0;

    int32_t ticks = getEncoderTicks();
    int32_t deltaTicks = ticks - lastTicks;
    lastTicks = ticks;

    if (!distInitialized || deltaTicks == 0) return;

    float ds = (float)deltaTicks * MM_PER_TICK;

    // Si el signo sale invertido en la práctica, cambia ds por -ds
    float alpha = radians((float)wrap180(yaw - setPointIMU));

    float dForward = ds * cos(alpha);
    float dLeft    = ds * sin(alpha);

    estFront -= dForward;
    estRight += dLeft;
    estLeft  -= dLeft;

    estFront = constrain(estFront, 0.0f, (float)TOF_MAX_VALID_MM);
    estLeft  = constrain(estLeft,  0.0f, (float)TOF_MAX_VALID_MM);
    estRight = constrain(estRight, 0.0f, (float)TOF_MAX_VALID_MM);
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

                    // Si una lectura pasa de 3000 mm, se descarta y se conserva la anterior
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

bool followingLeftWall() {
    return directionChosen && turnDirection == -1;
}

uint16_t getFollowWallDistance() {
    if (followingLeftWall()) {
        return left;
    }

    return right;
}

int getWallServoSign() {
    // Pared derecha:
    // Si está lejos de la pared derecha, errorWall positivo debe mandar servo hacia la derecha.
    // En este robot eso corresponde a restar wallTerm.
    //
    // Pared izquierda:
    // Si está lejos de la pared izquierda, errorWall positivo debe mandar servo hacia la izquierda.
    // En este robot eso corresponde a sumar wallTerm.
    if (followingLeftWall()) {
        return +1;
    }

    return -1;
}

bool wallDistanceAllowedForSteering(uint16_t wallDist) {
    // Caso especial antes de saber el sentido.
    // Al inicio se sigue la derecha. Si en realidad el sentido correcto era horario,
    // puede perder esa pared y brincar de ~200 a valores grandes.
    // En ese caso se usa solo IMU para evitar un giro brusco.
    if (!directionChosen) {
        if (lastWallDistValidForJump) {
            int jump = abs((int)wallDist - (int)lastWallDistForJump);

            if (jump > WALL_JUMP_THRESHOLD_MM) {
                return false;
            }
        }

        // Solo actualizamos la referencia si la lectura parece usable.
        if (wallDist >= wallMinValid && wallDist <= wallMaxValid) {
            lastWallDistForJump = wallDist;
            lastWallDistValidForJump = true;
        }
    }

    if (wallDist < wallMinValid || wallDist > wallMaxValid) {
        return false;
    }

    return true;
}

void updateSteering(int yaw) {
    errorIMU = angleErrorDeg(setPointIMU, yaw);

    float dtIMU = max(1.0f, (float)lastReadIMU);
    float dErrIMU = (errorIMU - lastErrorIMU) / dtIMU;
    float imuTerm = kpIMU * errorIMU + kdIMU * dErrIMU;
    lastErrorIMU = errorIMU;
    lastReadIMU = 0;

    float wallTerm = 0.0f;
    uint16_t wallDist = getFollowWallDistance();

    bool useWall = wallDistanceAllowedForSteering(wallDist);

    if (useWall) {
        float limitedAngle = constrain((float)errorIMU, -35.0f, 35.0f);
        float theta = radians(limitedAngle);

        // Distancia perpendicular corregida
        float wallCorrected = (float)wallDist * cos(theta);

        int errorWall = (int)wallCorrected - setPointWall;
        errorWall = constrain(errorWall, -maxWallError, maxWallError);

        float wallWeight = 1.0f - min(abs(errorIMU) / 40.0f, 1.0f);

        float dtWall = max(1.0f, (float)lastReadWall);
        float dErrWall = (errorWall - lastErrorWall) / dtWall;

        wallTerm = wallWeight * (kpWall * errorWall + kdWall * dErrWall);

        lastErrorWall = errorWall;
        lastReadWall = 0;
    } else {
        // Solo IMU.
        // También evitamos que el derivativo de pared guarde un error viejo.
        lastErrorWall = 0.0f;
        lastReadWall = 0;
    }

    int wallServoSign = getWallServoSign();

    if (useWall && wallTerm > 30) {
        int servoCmd = constrain((int)(servoCenter + wallServoSign * wallTerm), servoMin, servoMax);
        outError = abs(90 - servoCmd);
        servo.write(servoCmd);
    }
    else {
        int servoCmd = constrain((int)(servoCenter + imuTerm + wallServoSign * wallTerm), servoMin, servoMax);
        outError = abs(90 - servoCmd);
        servo.write(servoCmd);
    }
}

void startTurnByDirection() {
    setPointIMU = wrap180(setPointIMU + (90 * turnDirection));
    turning = true;
    turnArmed = false;

    resetWallJumpFilter();
}

void startDirectionChoice() {
    choosingDirection = true;
    directionChoiceTimer = 0;
    turnArmed = false;

    motorStop();
    servo.write(servoCenter);

    resetWallJumpFilter();

    tone(BUZZER, 1100, 100);
}

void finishDirectionChoiceAndStartTurn() {
    choosingDirection = false;
    directionChosen = true;

    if (left >= right) {
        turnDirection = 1;   // antihorario: gira izquierda y sigue pared derecha
        tone(BUZZER, 1500, 100);
    } else {
        turnDirection = -1;  // horario: gira derecha y sigue pared izquierda
        tone(BUZZER, 700, 100);
    }

    resetWallJumpFilter();

    startTurnByDirection();
    turnCount++;
}

void updateDirectionChoice() {
    motorStop();
    servo.write(servoCenter);

    if (directionChoiceTimer >= DIRECTION_CHOICE_WAIT_MS) {
        finishDirectionChoiceAndStartTurn();
    }
}

void updateTurn(int yaw) {
    int turnError = angleErrorDeg(setPointIMU, yaw);

    int servoCmd = constrain((int)(servoCenter + kpTurn * turnError), servoMin, servoMax);
    servo.write(servoCmd);

    if (abs(turnError) <= turnFinishError) {
        turning = false;
    }
}

int calcSpeed() {
    if (!turning) {
        int reductionError = outError * rP;
        float disVel = constrain(front / 2000.0f, 0.0f, 1.0f) * 70.0f;
        int outSpeed = 100 + (int)disVel - reductionError;
        return constrain(outSpeed, minPWM, maxPWM);
    } else {
        return 80;
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
    Serial4.begin(115200);    // IMU
    Serial5.begin(2000000);   // TOF UART

    pixels.begin();
    pixels.clear();
    pixels.show();

    servo.attach(SERVOMOTOR);
    servo.write(servoCenter);

    imu.begin(Serial4);

    motorEnc.write(0);

    delay(1000);
}

void loop() {
    while (butState == 0) {
        if (digitalRead(BUTTON) == 1) {
            butState = 1;
            tone(BUZZER, 700, 500);
            delay(300);
        }

        bool newTofStart = readTOFPacket(Serial5, front, left, right);
        if (newTofStart) {
            syncEstimateFromCurrentDistances();
            pixels.setPixelColor(0, pixels.Color(50, 50, 50));
        } else {
            pixels.setPixelColor(0, pixels.Color(0, 0, 0));
        }
        pixels.show();
        Serial.println(front);
    }

    imu.update();
    int imuRead = wrap180(imu.getYaw());

    // Primero estimar con encoder + IMU
    updateEstimatedDistances(imuRead);
    if (distInitialized) {
        publishEstimatedDistances();
    }

    // Luego corregir con lectura real si llega
    bool newTof = readTOFPacket(Serial5, front, left, right);
    if (newTof) {
        syncEstimateFromCurrentDistances();
    }

    if (choosingDirection) {
        updateDirectionChoice();

        pixels.setPixelColor(0, pixels.Color(20, 20, 50));
        pixels.show();

        return;
    }

    // Rearmar el giro solo cuando el frente vuelva a estar despejado
    if (front > frontTurnThreshold + 100) {
        turnArmed = true;
    }

    // Motor hacia adelante
    motorForwardPWM(calcSpeed());

    if (finish == 1) {
        motorStop();
    }
    else if (turnCount == 12 && !turning) {
        if (!finalAdvanceStarted) {
            finalTimer = 0;
            finalAdvanceStarted = true;
        }

        if (finalTimer < 1500) {
            updateSteering(imuRead);
            motorForwardPWM(calcSpeed());
        }
        else {
            finish = 1;
        }
    }
    else if (!turning) {
        updateSteering(imuRead);

        if (turnArmed && front < frontTurnThreshold && abs(errorIMU) < 10) {
            if (!directionChosen) {
                startDirectionChoice();
            } else {
                startTurnByDirection();
                turnCount++;
                tone(BUZZER, 1500, 100);
            }
        }
    } else {
        updateTurn(imuRead);
    }

    if (newTof) {
        if (followingLeftWall()) {
            pixels.setPixelColor(0, pixels.Color(50, 0, 50));   // siguiendo izquierda
        } else {
            pixels.setPixelColor(0, pixels.Color(0, 0, 50));    // siguiendo derecha
        }
    } else {
        pixels.setPixelColor(0, pixels.Color(0, 0, 0));
    }

    pixels.show();
}