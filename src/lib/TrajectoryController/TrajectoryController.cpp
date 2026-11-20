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
    float obstacleSecurityRadiusPx,
    float distanceToObstaclePx,
    float obstacleDepthY)
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

    const float depthGain = updateDepthGain(obstacleDepthY, direction, deltaTimeSeconds);
    if (depthGain != 1.0f && direction * evasionAngularError > 0.0f) {
        // Extra proportional evasion only: neither heading gain nor D sees the depth boost.
        // Once the tangent reverses, preserve the old return correction without amplifying it.
        const float evasionContribution =
            evasionWeight * TAN_EVASION_EVASION_GAIN * evasionAngularError;
        output += TAN_EVASION_KP * (depthGain - 1.0f) * evasionContribution;
    }

    lastEvasionError = totalAngularError;
    evasionInitialized = true;
    deltaEvasion = 0;
    lastTangentialOutputDeg = output;

    return output;
}

float TrajectoryController::updateDepthGain(float obstacleDepthY, float direction,
                                           float deltaTimeSeconds)
{
    const float startY = TAN_EVASION_DEPTH_START_Y_PX;
    const float fullY = TAN_EVASION_DEPTH_FULL_Y_PX;
    const float maxGain = TAN_EVASION_DEPTH_GAIN_MAX;
    const float smoothMs = TAN_EVASION_DEPTH_SMOOTH_MS;
    if (!isfinite(obstacleDepthY) || obstacleDepthY < 0.0f || obstacleDepthY >= VISION_HEIGHT ||
        !isfinite(startY) || !isfinite(fullY) || startY <= fullY ||
        !isfinite(maxGain) || maxGain <= 1.0f || !isfinite(smoothMs) || smoothMs < 0.0f) {
        resetDepthGain();
        return 1.0f;
    }

    const float t = constrain((startY - obstacleDepthY) / (startY - fullY), 0.0f, 1.0f);
    const float target = 1.0f + (maxGain - 1.0f) * t * t * (3.0f - 2.0f * t);
    if (!depthGainInitialized || direction != lastDepthDirection || smoothMs == 0.0f) {
        // A newly acquired close block gets full applicable boost without a startup delay.
        lastDepthGain = target;
    }
    else if (deltaTimeSeconds > 0.0f) {
        const float alpha = 1.0f - expf(-deltaTimeSeconds * 1000.0f / smoothMs);
        lastDepthGain += alpha * (target - lastDepthGain);
    }
    lastDepthGain = constrain(lastDepthGain, 1.0f, maxGain);
    lastDepthDirection = direction;
    depthGainInitialized = true;
    return lastDepthGain;
}

void TrajectoryController::resetDepthGain()
{
    lastDepthGain = 1.0f;
    depthGainInitialized = false;
    lastDepthDirection = 0.0f;
}

void TrajectoryController::resetTangentEvasion(float currentError)
{
    resetDepthGain();
    lastEvasionError = currentError;
    filteredEvasionDerivative = 0.0f;
    evasionInitialized = false;
    deltaEvasion = 0;
}
