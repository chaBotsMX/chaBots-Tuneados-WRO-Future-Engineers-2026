/**
 * @file Robot.cpp
 * @brief Robot instance to make it clearer.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 11/08/26
 */

 #include "Robot.h"

 Robot::Robot()
    : xiao(Serial5, XIAO_BAUD_RATE){}

 void Robot::beginComms(){
    Serial.begin(PC_SERIAL_BAUD_RATE);
    Serial4.begin(IMU_BAUD_RATE); // IMU
    Serial5.begin(XIAO_BAUD_RATE); // RX, TX

    imu.begin(IMU_SERIAL);

 }


bool Robot::updateSensors(){
    xiao.readData();
    if(xiao.available()){
        data = xiao.getData();
        if(data.front < 4000 ) {
            validData.front = data.front;
        }
        if(data.left < 4000 ) {
            validData.left = data.left;
        }
        if(data.right < 4000 ) {
            validData.right = data.right;
        }
        if(data.back < 4000 ) {
            validData.back = data.back;
        }
        return true;
  }
  return false;
}

bool Robot::followWallByCM(){
    float stanleyTheta = tc.stanley(wallDistance - validData.left,imu.getError(),float(move.getCurrentSpeed()), 0.002f, 0.5f);
    ackermann.setSteeringAngle(stanleyTheta);
    if(move.updateCM()){
        taskStatus = 0;
    }
}

bool Robot::followWallUntilWall(){
    float stanleyTheta = tc.stanley(wallDistance - validData.left,imu.getError(),float(move.getCurrentSpeed()), 0.002f, 0.5f);
    ackermann.setSteeringAngle(stanleyTheta);
    move.driveAtSpeed(30,1,0.1,move.getCurrentSpeed());

    frontDistance = validData.front;

    if(frontDistance < 1000){
        taskStatus = 0;
    }
}

void Robot::taskFollowWallByCm(float travelCM,float speed,float acceleration,float deacceleration, float initSpeed, float finalSpeed){
    move.setTask(travelCM,speed,acceleration,deacceleration,initSpeed,finalSpeed);
    taskStatus = 1;
}

void Robot::taskFollowWallUntilWall(){
    taskStatus = 0;
}

void Robot::executeTask(){
    if(taskStatus == 1){
        followWallByCM();
    }
    else if(taskStatus == 2){
        followWallUntilWall();
    }
}