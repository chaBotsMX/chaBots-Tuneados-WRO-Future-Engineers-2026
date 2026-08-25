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

void Robot::steerByStanley(float stanleyWallGain,float stanleyHeadingGain){
    if(this->direction == DIRECTIONS::CLOCKWISE){
        float stanleyTheta = tc.stanley(wallDistance - validData.left,-imu.getError(),float(move.getCurrentSpeed()),    stanleyWallGain, stanleyHeadingGain);
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

void Robot::decideDir(){
    if(data.right > data.left){
        direction = DIRECTIONS::CLOCKWISE;
    }
    else{
        direction = DIRECTIONS::ANTICLOCKWISE;
    }
}

void Robot::selectTask(){
    //verify if rutine is ended
    if(finish == true){
        return;
    }    
    if(taskStatus == TASK::UNDEFINED){
        taskStatus = TASK::GOSTRAIGHTTOEDGE;
        move.setTask(80,50,30,30,0,0);
    }
    else if(taskStatus == TASK::GOSTRAIGHTTOEDGE){
        float error = imu.getError();
        float theta = error * 1.0f;
        ackermann.setSteeringAngle(theta);
        if(move.updateCM()){
            taskStatus = TASK::GETCLOSETOEDGE;            
        }
    }
    else if( taskStatus == TASK::GETCLOSETOEDGE){
        move.driveAtPWM(EDGING_PWM);
        if(validData.front < EDGING_TARGET_DISTANCE){
            decideDir(); 
            move.driveAtPWM(INITIAL_REVERSE_PWM);
            delay(INITIAL_REVERSE_TIME);
            taskStatus = TASK::OPENTURN;
            int dirMultiplier = 0;
            direction == DIRECTIONS::ANTICLOCKWISE? dirMultiplier = -1: dirMultiplier = 1;
            int newOffset = 90 * dirMultiplier;
            initialSetPoint = wrap180(initialSetPoint + newOffset);
            imu.setSetPoint(initialSetPoint);
        }
    }
    else if(taskStatus == TASK::OPENTURN){
        float error = imu.getError();
        if(abs(error) < 5){
            if(lapCount != 11){
                taskStatus = TASK::FOLLOWWALL;
                move.setTask(110,80,30,30,0,0);
                lapCount++;
            }
            else{
                taskStatus = TASK::OPENENDING;
                move.setTask(40,30,30,30,0,0);
            }
        }
        float theta = 0;
        if(direction == DIRECTIONS::ANTICLOCKWISE){ 
            theta = imu.getError() * 0.5;
        }
        else{
            theta = imu.getError() * 0.5; 
        }
        ackermann.setSteeringAngle(theta);
        move.driveAtPWM(-100);
    }
    else if(taskStatus == TASK::FOLLOWWALL){
        if(direction == DIRECTIONS::CLOCKWISE){
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
            taskStatus = TASK::FOLLOWUNTILEDGE;
        }
    }
    else if(taskStatus == TASK::FOLLOWUNTILEDGE){
        steerByStanley(0.002f, 1.0f);

        frontDistance = validData.front;
        int innerWall = 0;
        if(direction == DIRECTIONS::ANTICLOCKWISE){
            innerWall = data.left;
        }
        else{
            innerWall = data.right;
        }
        if(frontDistance < 600 && frontDistance > 400 && innerWall > 3000){
            taskStatus = TASK::OPENTURN;
            int dirMultiplier = 0;
            direction == DIRECTIONS::ANTICLOCKWISE? dirMultiplier = -1: dirMultiplier = 1;
            int newOffset = 90 * dirMultiplier;
            initialSetPoint = wrap180(initialSetPoint + newOffset);
            imu.setSetPoint(initialSetPoint);
            return;
        }
        else if( frontDistance > 600 || innerWall < 3000){
            move.driveAtSpeed(70,1,0.1,move.getCurrentSpeed());
            return;
        }
        move.driveAtPWM(100);
        delay(100);
        validData.front = 3000;
    }
    else if(taskStatus == TASK::OPENENDING){
        if(direction == DIRECTIONS::CLOCKWISE){
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
            finish = true;
        }        
    }
}
