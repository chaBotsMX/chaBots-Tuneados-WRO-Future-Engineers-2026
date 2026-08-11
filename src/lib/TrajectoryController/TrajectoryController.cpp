/**
 * @file TrayectoryController.cpp
 * @brief Library to manage controllers for movement like stanley or pure pursuit.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 11/08/26
 */

 #include "TrajectoryController.h"

 TrajectoryController::TrajectoryController(){}

float TrajectoryController::stanley(float wallError, float imuError, float speed, float stanleyGain, float headingGain){

    float angularError = wrap180(imuError);

    float lateralCorrection = degrees(atan2(stanleyGain * wallError,speed + 1.0f));
    return headingGain * angularError + lateralCorrection;
}
