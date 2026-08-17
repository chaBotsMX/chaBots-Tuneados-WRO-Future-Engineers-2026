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
    if(direction == -1){
        float stanleyTheta = tc.stanley(wallDistance - validData.left,-imu.getError(),float(move.getCurrentSpeed()), 0.002f, 1.0f);
        ackermann.setSteeringAngle(-stanleyTheta);
        Serial.print("stanleyTheta: ");
        Serial.println(stanleyTheta);
    }
    else{
        float stanleyTheta = tc.stanley(wallDistance - validData.right,imu.getError(),float(move.getCurrentSpeed()), 0.002f, 1.0f);
        ackermann.setSteeringAngle(stanleyTheta);
        Serial.print("stanleyTheta: ");
        Serial.println(stanleyTheta);
    }
    if(move.updateCM()){
        taskStatus = 0;
        return true;
    }
    return false;
}

bool Robot::followWallUntilWall(){
    steerByStanley(0.002f, 1.0f);

    frontDistance = validData.front;
    int innerWall = 0;
    if(direction == 1){
        innerWall = data.left;
    }
    else{
        innerWall = data.right;
    }
    if(frontDistance < 600 && frontDistance > 400 && innerWall > 3000){
        taskStatus = 0;
        return true;
    }
    else if( frontDistance > 600 || innerWall < 3000){
        move.driveAtSpeed(70,1,0.1,move.getCurrentSpeed());

        return false;
    }
    move.driveAtPWM(100);
    delay(100);
    validData.front = 3000;
    return false;
}

void Robot::taskFollowWallByCm(float travelCM,float speed,float acceleration,float deacceleration, float initSpeed, float finalSpeed){
    move.setTask(travelCM,speed,acceleration,deacceleration,initSpeed,finalSpeed);
    taskStatus = 1;
}

void Robot::taskFollowWallUntilWall(){
    taskStatus = 2;
}

void Robot::taskTurn(){
    taskStatus = 3;
    int newOffset = 90 * direction;
    initialSetPoint = wrap180(initialSetPoint + newOffset);
    imu.setSetPoint(initialSetPoint);

}

void Robot::executeTask(){
    if(taskStatus == 1){
        followWallByCM();
    }
    else if(taskStatus == 2){
        followWallUntilWall();
    }
    else if(taskStatus == 3){
        turn();
    }
    else if(taskStatus == 4){
        goStraighUntilEdge();
    }
    else if(taskStatus == 5){
        goStraightByIMUCM();
    }
    else if(taskStatus == 6){
        goToEdge();
    }
}

void Robot::printData(){
    Serial.print("validData { front: ");
    Serial.print(validData.front);
    Serial.print(", left: ");
    Serial.print(validData.left);
    Serial.print(", right: ");
    Serial.print(validData.right);
    Serial.print(", back: ");
    Serial.print(validData.back);
    Serial.println(" }");
}

void Robot::turn(){
    float error = imu.getError();
    if(abs(error) < 5){
        taskStatus = 0;
    }
     float theta = 0;
    if(direction == 1){ 
        theta = imu.getError() * 0.5;
    }
    else{
        theta = imu.getError() * 0.5; 
    }
    ackermann.setSteeringAngle(theta);
    move.driveAtPWM(-100);

}

void Robot::goStraighUntilEdge(){
    move.driveAtPWM(-50);
    float error = imu.getError();
    float theta = error * 1.0f;
    ackermann.setSteeringAngle(theta);
    if(move.updateCM()){
        taskStatus = 3;
    }
}

void Robot::taskGoStraighUntilEdge(){
    move.setTask(80,50,30,30,0,0);
    taskStatus = 4;
}

void Robot::steerByStanley(float stanleyWallGain,float stanleyHeadingGain){
    if(this->direction == -1){
    float stanleyTheta = tc.stanley(wallDistance - validData.left,-imu.getError(),float(move.getCurrentSpeed()), stanleyWallGain, stanleyHeadingGain);
    ackermann.setSteeringAngle(-stanleyTheta);
    }
    else{
    float stanleyTheta = tc.stanley(wallDistance - validData.right,imu.getError(),float(move.getCurrentSpeed()), stanleyWallGain, stanleyHeadingGain);
    ackermann.setSteeringAngle(stanleyTheta);
    }
   

}

void Robot::beginData(){
    this->validData.front = MAX_VALID_DISTANCE;
    this->validData.left = MAX_VALID_DISTANCE;
    this->validData.right = MAX_VALID_DISTANCE;


}

void Robot::begin(){
    this->beginComms();
    delay(1000);
    this->ui.begin();
    this->ackermann.begin();
    this->beginData();

    while (this->ui.buttonRead() == false) {
    if(this->updateSensors() == true){
    this->ui.neoColor(0,255,0);
    }
    if (this->ui.buttonRead() == true) {

        this->ui.neoColor(0,0,255);
        this->ui.buzzSound(1);
    }
    }
    this->imu.setSetPoint(0); 
}

bool Robot::evadeUntilEdge(){
    // si no esta viendo pilar y la distancia al frente es menor que 1000 y mayor que 500
    if(validData.front < 1000  && validData.front < 500 && !isSeeingObject()){
        return 1;
    }
    else if(!isSeeingObject()){ // si no esta viendo pilar sigue derecho
        goStraightByIMUCM();
        return 0;
    }
    // si esta viendo pilar evadir

    // si es verde evade derecha
    // si es rojo evade izquierda
    return 0;
}

bool Robot::goStraightByIMUCM(){
    float error = imu.getError();
    float theta = error * 1.0f;
    ackermann.setSteeringAngle(theta);
    
    if(move.updateCM()){
        taskStatus = 0;
        return true;
    }
    return false;
}

void Robot::taskGoStraightByIMUCM(float travelCM,float speed,float acceleration,float deacceleration, float initSpeed, float finalSpeed){
    move.setTask(travelCM,speed,acceleration,deacceleration,initSpeed,finalSpeed);
    taskStatus = 5;
}

bool Robot::goToEdge(){
    move.driveAtPWM(-70);
    if(validData.front < 300){
        ui.buzzSound(1);
        decideDir(); 
        move.driveAtPWM(50);
        delay(1000);
        taskStatus = 0;   
   
        return true;
    }
    return false;
}

void Robot::taskGoToEdge(){
    taskStatus = 6;
}

void Robot::decideDir(){
    if(data.right > data.left){
        direction = -1;
    }
    else{
        direction = 1;
    }
}