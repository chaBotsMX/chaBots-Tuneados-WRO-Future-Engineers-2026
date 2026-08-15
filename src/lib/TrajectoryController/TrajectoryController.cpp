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

float TrajectoryController::tangentEvasion(float imuError, float direction, float obstacleAngle,float obstacleSecurityRadio, float distanceToObstacle){
    float orientationAngularError = imuError;
    float evasionAngularError = obstacleAngle + (direction * degrees(asinf(obstacleSecurityRadio/distanceToObstacle))); 

    //TODO: add constants
    float minActivationDistance = 100;
    float maxActivarionDistance = 30;

    float evasionGain = 1;
    float orientationGain = 1;
    float proportionalTangentEvasion = 1;
    float derivativeProportionalEvasion = 1;

    float evasionWeight = constrain(minActivationDistance - distanceToObstacle / minActivationDistance - maxActivarionDistance, 0,1);

    float totalAngularError = (orientationAngularError * orientationGain) + (evasionAngularError * evasionGain);

    float outPut = (totalAngularError * proportionalTangentEvasion) + (totalAngularError - lastEvasionError / deltaEvasion);

    deltaEvasion = 0;
    lastEvasionError = totalAngularError;
    return outPut;
}