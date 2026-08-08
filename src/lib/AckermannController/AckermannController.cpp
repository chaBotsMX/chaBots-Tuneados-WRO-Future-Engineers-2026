/**
 * @file AckermannController.cpp
 *
 * @author Roy Barron / chaBotsMX
 * @date 08/07/26
 */

#include "AckermannController.h"

AckermannController::AckermannController(uint8_t servoPin)
{
    steeringServo.attach(servoPin);
    steeringServo.write(IDLE_STEERING_ANGLE);
}

float AckermannController::interpolateServoOffset(float ackermannAngle)
{
    float angle = fabs(ackermannAngle);

    if (angle <= steeringTable[0].ackermannAngle) {
        return steeringTable[0].servoOffset;
    }

    if (angle >= steeringTable[MAP_SIZE - 1].ackermannAngle) {
        return steeringTable[MAP_SIZE - 1].servoOffset;
    }

    for (int i = 0; i < MAP_SIZE - 1; i++) {

        float ackermannMin =
            steeringTable[i].ackermannAngle;

        float ackermannMax =
            steeringTable[i + 1].ackermannAngle;

        if (
            angle >= ackermannMin &&
            angle <= ackermannMax
        ) {
            float servoMin =
                steeringTable[i].servoOffset;

            float servoMax =
                steeringTable[i + 1].servoOffset;

            float t =
                (angle - ackermannMin) /
                (ackermannMax - ackermannMin);

            return servoMin +
                   t * (servoMax - servoMin);
        }
    }

    return 0.0f;
}

void AckermannController::setSteeringAngle(float angle)
{
    angle = constrain(
        angle,
        -MAX_ACKERMANN_ANGLE,
        MAX_ACKERMANN_ANGLE
    );

    float servoOffset =
        interpolateServoOffset(angle);

    if (angle < 0.0f) {
        servoOffset = -servoOffset;
    }

    steeringServo.write(
        IDLE_STEERING_ANGLE + servoOffset
    );
}

void AckermannController::steerByRawAngle(float rawAngle)
{
    rawAngle = constrain(
        rawAngle,
        -MAX_SERVO_OFFSET,
        MAX_SERVO_OFFSET
    );

    steeringServo.write(
        IDLE_STEERING_ANGLE + rawAngle
    );
}