/**
 * @file TrayectoryController.cpp
 * @brief Library to manage controllers for movement like stanley or pure pursuit.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 11/08/26
 */

 #include "TrajectoryController.h"

 TrajectoryController::TrajectoryController()
    : deltaEvasion(0)
 {}

float TrajectoryController::stanley(float wallError, float imuError, float speed, float stanleyGain, float headingGain){

    float angularError = wrap180(imuError);

    float lateralCorrection = degrees(atan2(stanleyGain * wallError,speed + 1.0f));
    return headingGain * angularError + lateralCorrection;
}

float TrajectoryController::tangentEvasion(
    float imuError,
    float direction,
    float obstacleAngle,
    float obstacleSecurityRadius,
    float distanceToObstacle)
{
    if (distanceToObstacle <= 0.0f) {
        resetTangentEvasion(imuError);
        return imuError;
    }

    direction = direction >= 0.0f ? 1.0f : -1.0f;

    float tangentRatio = constrain(
        obstacleSecurityRadius / distanceToObstacle,
        0.0f,
        1.0f
    );

    float tangentAngle = degrees(asinf(tangentRatio));

    float orientationAngularError = imuError;

    float evasionAngularError =
        obstacleAngle + direction * tangentAngle;

    float evasionWeight = constrain(
        (TAN_EVASION_ACTIVATION_DISTANCE_MM - distanceToObstacle) /
        (TAN_EVASION_ACTIVATION_DISTANCE_MM - TAN_EVASION_FULL_EVASION_DISTANCE_MM),
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

void TrajectoryController::resetTangentEvasion(float currentError)
{
    lastEvasionError = currentError;
    filteredEvasionDerivative = 0.0f;
    evasionInitialized = false;
    deltaEvasion = 0;
}
