// Frozen pre-depth-gain controller: regression oracle, not production code.
#pragma once
#include <Arduino.h>
#include "ControlValues.h"
#include "Utils.h"
class LegacyTangentEvasion {
public:
    float tangentEvasion(float imuError, float direction, float obstacleAngle,
                        float obstacleSecurityRadiusPx, float distanceToObstaclePx);
    void resetTangentEvasion(float currentError = 0.0f);
private:
    float lastEvasionError = 0.0f;
    float filteredEvasionDerivative = 0.0f;
    bool evasionInitialized = false;
    elapsedMillis deltaEvasion;
};
float LegacyTangentEvasion::tangentEvasion(
    float imuError,
    float direction,
    float obstacleAngle,
    float obstacleSecurityRadiusPx,
    float distanceToObstaclePx)
{
    if (distanceToObstaclePx <= 0.0f) {
        resetTangentEvasion(imuError);
        return imuError;
    }

    direction = direction >= 0.0f ? 1.0f : -1.0f;

    float tangentRatio = constrain(
        obstacleSecurityRadiusPx / distanceToObstaclePx,
        0.0f,
        1.0f
    );

    float tangentAngle = degrees(asinf(tangentRatio));

    float orientationAngularError = imuError;

    float evasionAngularError =
        obstacleAngle + direction * tangentAngle;

    float evasionWeight = constrain(
        (TAN_EVASION_ACTIVATION_DISTANCE_PX - distanceToObstaclePx) /
        (TAN_EVASION_ACTIVATION_DISTANCE_PX - TAN_EVASION_FULL_EVASION_DISTANCE_PX),
        0.0f,
        1.0f
    );

    evasionWeight = evasionWeight * evasionWeight *
                    (3.0f - 2.0f * evasionWeight);

    float totalAngularError =
        TAN_EVASION_ORIENTATION_GAIN * orientationAngularError +
        evasionWeight * TAN_EVASION_EVASION_GAIN * evasionAngularError;

    float derivative = 0.0f;
    const float deltaTimeSeconds = static_cast<float>(deltaEvasion) / 1000.0f;

    if (evasionInitialized && deltaTimeSeconds > 0.001f) {
        float errorChange = wrapAngleDegrees(
            totalAngularError - lastEvasionError
        );
        derivative = errorChange / deltaTimeSeconds;
    }

    filteredEvasionDerivative += TAN_EVASION_DERIVATIVE_FILTER *
        (derivative - filteredEvasionDerivative);

    float output =
        TAN_EVASION_KP * totalAngularError +
        TAN_EVASION_KD * filteredEvasionDerivative;

    lastEvasionError = totalAngularError;
    evasionInitialized = true;
    deltaEvasion = 0;

    return output;
}

void LegacyTangentEvasion::resetTangentEvasion(float currentError)
{
    lastEvasionError = currentError;
    filteredEvasionDerivative = 0.0f;
    evasionInitialized = false;
    deltaEvasion = 0;
}
