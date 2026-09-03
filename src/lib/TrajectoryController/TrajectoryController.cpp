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
    // Todas estas distancias deben estar en la misma unidad. Robot usa pixeles
    // del plano de imagen porque la camara no entrega una distancia en cm.
    constexpr float activationDistance = 200.0f;
    constexpr float fullEvasionDistance = 160.0f;

    // Ganancias de combinación
    constexpr float orientationGain = 2.0f;
    constexpr float evasionGain = 4.0f;

    // Ganancias PD
    constexpr float kp = 1.0f;
    constexpr float kd = 0.02f;
    constexpr float derivativeFilter = 0.20f;

    // Protección contra división entre cero
    if (distanceToObstacle <= 0.0f) {
        resetTangentEvasion(imuError);
        return imuError;
    }

    // direction debe ser +1 o -1
    direction = direction >= 0.0f ? 1.0f : -1.0f;

    float tangentRatio = constrain(
        obstacleSecurityRadius / distanceToObstacle,
        0.0f,
        1.0f
    );

    // Todo se trabaja en grados
    float tangentAngle = degrees(asinf(tangentRatio));

    float orientationAngularError = imuError;

    float evasionAngularError =
        obstacleAngle + direction * tangentAngle;

    float evasionWeight = constrain(
        (activationDistance - distanceToObstacle) /
        (activationDistance - fullEvasionDistance),
        0.0f,
        1.0f
    );

    // Smoothstep evita un cambio brusco al cruzar la distancia de activacion.
    evasionWeight = evasionWeight * evasionWeight *
                    (3.0f - 2.0f * evasionWeight);

    float totalAngularError =
        orientationGain * orientationAngularError +
        evasionWeight * evasionGain * evasionAngularError;

    float derivative = 0.0f;
    const float deltaTimeSeconds = static_cast<float>(deltaEvasion) / 1000.0f;

    if (evasionInitialized && deltaTimeSeconds > 0.001f) {
        float errorChange = wrapAngleDegrees(
            totalAngularError - lastEvasionError
        );
        derivative = errorChange / deltaTimeSeconds;
    }

    filteredEvasionDerivative += derivativeFilter *
        (derivative - filteredEvasionDerivative);

    float output =
        kp * totalAngularError +
        kd * filteredEvasionDerivative;

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
