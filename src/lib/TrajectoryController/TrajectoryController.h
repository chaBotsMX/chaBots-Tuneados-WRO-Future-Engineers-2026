/**
 * @file TrayectoryController.h
 * @brief Library to manage controllers for movement like stanley or pure pursuit.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 11/08/26
 */

#ifndef TRAJECTORYCONTROLLER_H
#define TRAJECTORYCONTROLLER_H

#include <Arduino.h>
#include "Utils.h"

class TrajectoryController{
public:
    TrajectoryController();
    float lastStanleyOutputDeg = 0.0f;
    float lastTangentialOutputDeg = 0.0f;
    float stanley(float wallError, float imuError, float speed, float stanleyGain, float headingGain);
    float tangentEvasion(float imuError,
    float direction,
    float obstacleAngle,
    float obstacleSecurityRadius,
    float distanceToObstacle);
    void resetTangentEvasion(float currentError = 0.0f);
private:
    float lastEvasionError = 0.0f;
    float filteredEvasionDerivative = 0.0f;
    bool evasionInitialized = false;
    elapsedMillis deltaEvasion;

};

#endif
