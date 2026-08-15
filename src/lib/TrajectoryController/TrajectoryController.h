/**
 * @file TrayectoryController.h
 * @brief Library to manage controllers for movement like stanley or pure pursuit.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 11/08/26
 */

 #ifndef TRAJECTORYCONTROLLLER_H
 #define TRAJECTORYCONTROLLER_H

#include <Arduino.h>
#include "Utils.h"

class TrajectoryController{
public:
    TrajectoryController();
    float stanley(float wallError, float imuError, float speed, float stanleyGain, float headingGain);
    float tangentEvasion(float imuError, float direction, float obstacleAngle,float obstacleSecurityRadio, float distanceToObstacle);
private:
    float lastEvasionError = 0;

    elapsedMillis deltaEvasion;

};

 #endif
