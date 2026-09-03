/**
 * @file Robot.cpp
 * @brief Robot instance to make it clearer.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 11/08/26
 */

 #include "Robot.h"

 Robot::Robot()
    :  tofs(SPI, CS_FRONT, CS_RIGHT, CS_LEFT, CS_BACK){}

 void Robot::beginComms(){
    Serial.begin(PC_SERIAL_BAUD_RATE);
    IMU_SERIAL.begin(IMU_BAUD_RATE); // IMU
    Serial5.begin(XIAO_BAUD_RATE); // TX
    CAM_SERIAL.begin(CAM_SERIAL_BAUDRATE);

    delay(100);
    move.begin();
    delay(100);
    imu.begin(IMU_SERIAL);
    delay(1000);
    if (!tofs.begin(60)) {
        Serial.println("Error, pls reboot");
        while (true) {  }
    }
    Serial.println("All TOFs ready");
    delay(1000);
 }

void Robot::updateCam(){
    camera.update(CAM_SERIAL,vision);
}

bool Robot::updateSensors(){
    tofs.update();

    bool hasFreshData = false;
    hasFreshData |= updateSide(TOF4Walls::FRONT, data.front, validData.front, frontDataAge);
    hasFreshData |= updateSide(TOF4Walls::BACK, data.back, validData.back, backDataAge);
    hasFreshData |= updateSide(TOF4Walls::LEFT, data.left, validData.left, leftDataAge);
    hasFreshData |= updateSide(TOF4Walls::RIGHT, data.right, validData.right, rightDataAge);

    invalidateStaleSensorData();

    if (hasFreshData || displayRefreshAge >= 100) {
        refreshDebugDisplay();
    }

    return hasFreshData;
}

bool Robot::updateSide(TOF4Walls::Side side, uint16_t& rawDistance,
                       uint16_t& filteredDistance, elapsedMillis& dataAge){
    if (!tofs.hasFreshData(side)) {
        return false;
    }

    dataAge = 0;
    int16_t distance = tofs.getDistance(side);

    if (distance < 0) {
        rawDistance = INVALID_DISTANCE;
        filteredDistance = INVALID_DISTANCE;
        return true;
    }

    rawDistance = static_cast<uint16_t>(distance);
    filteredDistance = rawDistance < MAX_VALID_DISTANCE
        ? rawDistance
        : INVALID_DISTANCE;
    return true;
}

void Robot::invalidateStaleSensorData(){
    if (frontDataAge > SENSOR_DATA_TIMEOUT_MS) {
        data.front = INVALID_DISTANCE;
        validData.front = INVALID_DISTANCE;
    }
    if (backDataAge > SENSOR_DATA_TIMEOUT_MS) {
        data.back = INVALID_DISTANCE;
        validData.back = INVALID_DISTANCE;
    }
    if (leftDataAge > SENSOR_DATA_TIMEOUT_MS) {
        data.left = INVALID_DISTANCE;
        validData.left = INVALID_DISTANCE;
    }
    if (rightDataAge > SENSOR_DATA_TIMEOUT_MS) {
        data.right = INVALID_DISTANCE;
        validData.right = INVALID_DISTANCE;
    }
}

void Robot::refreshDebugDisplay(){
    ui.showDebug(taskName(taskStatus), taskName(previousTaskStatus),
                 data.front, data.left, data.right,
                 imu.getYaw(), imu.setPoint);
    displayRefreshAge = 0;
}

void Robot::printData(){
   /* Serial.print("validData { front: ");
    Serial.print(validData.front);
    Serial.print(", left: ");
    Serial.print(validData.left);
    Serial.print(", right: ");
    Serial.print(validData.right);
    Serial.print(", back: ");
    Serial.print(validData.back);
    Serial.println(" }");*/
    //Serial.println(move.controller.getDistanceMM());
    Serial.println(vision.obstacleX);
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
    this->validData.back = MAX_VALID_DISTANCE;
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

void Robot::decideDir(){
    if(data.right > data.left){
        direction = DIRECTIONS::CLOCKWISE;
    }
    else{
        direction = DIRECTIONS::COUNTERCLOCKWISE;
    }
}

const char* Robot::taskName(TASK task){
    switch (task) {
        case TASK::UNDEFINED:          return "UNDEFINED";
        case TASK::GO_STRAIGHT_TO_EDGE:return "GO_STRAIGHT";
        case TASK::GET_CLOSE_TO_EDGE:  return "GET_CLOSE";
        case TASK::FOLLOW_WALL:        return "FOLLOW_WALL";
        case TASK::FOLLOW_UNTIL_EDGE:  return "FOLLOW_EDGE";
        case TASK::OPEN_TURN:          return "OPEN_TURN";
        case TASK::OPEN_ENDING:        return "OPEN_END";
        case TASK::FINISHED:           return "FINISHED";
        case TASK::EVADE_UNTIL_EDGE:   return "EVADE";
        case TASK::APPROACH_BLUE_LINE: return "APPROACH_LINE";
        case TASK::REVERSE_AFTER_BLUE_LINE:return "REVERSE_LINE";
        case TASK::FORWARD_AFTER_REVERSE:return "FORWARD_LINE";
        default:                       return "UNKNOWN";
    }
}

void Robot::changeTask(TASK newTask){
    if (taskStatus == newTask) {
        return;
    }
    validData.front = INVALID_DISTANCE;

    previousTaskStatus = taskStatus;
    taskStatus = newTask;

    Serial.print("[STATE] ");
    Serial.print(taskName(previousTaskStatus));
    Serial.print(" -> ");
    Serial.println(taskName(taskStatus));
    refreshDebugDisplay();
}

float Robot::smoothSteeringCommand(float targetAngle){
    constexpr float maxSteeringSpeedDegreesPerSecond = 100.0f;

    targetAngle = constrain(
        targetAngle,
        -MAX_ACKERMANN_ANGLE,
        MAX_ACKERMANN_ANGLE
    );

    if(!steeringCommandInitialized){
        lastSteeringCommand = targetAngle;
        steeringCommandInitialized = true;
        steeringCommandAge = 0;
        return lastSteeringCommand;
    }

    const float deltaTimeSeconds =
        static_cast<float>(steeringCommandAge) / 1000.0f;
    if(deltaTimeSeconds <= 0.0f){
        return lastSteeringCommand;
    }

    const float maxChange =
        maxSteeringSpeedDegreesPerSecond * deltaTimeSeconds;
    const float requestedChange = targetAngle - lastSteeringCommand;
    lastSteeringCommand += constrain(requestedChange, -maxChange, maxChange);
    steeringCommandAge = 0;

    return lastSteeringCommand;
}


void Robot::selectTask(){
    //verify if rutine is ended
    if(finish == true){
        return;
    }    
    if(taskStatus == TASK::UNDEFINED){
        changeTask(TASK::GO_STRAIGHT_TO_EDGE);
        move.setTask(95,50,30,30,0,0);
    }
    else if(taskStatus == TASK::GO_STRAIGHT_TO_EDGE){
        float error = imu.getError();
        float theta = error * 1.0f;
        ackermann.setSteeringAngle(theta);
        if(move.updateCM()){
            changeTask(TASK::GET_CLOSE_TO_EDGE);
        }
    }
    else if( taskStatus == TASK::GET_CLOSE_TO_EDGE && validData.front != -1){
        move.driveAtPWM(EDGING_PWM);
        if(validData.front < EDGING_TARGET_DISTANCE){
            decideDir(); 
            move.driveAtPWM(INITIAL_REVERSE_PWM);
            delay(INITIAL_REVERSE_TIME);
            changeTask(TASK::OPEN_TURN);
            int dirMultiplier = 0;
            direction == DIRECTIONS::COUNTERCLOCKWISE? dirMultiplier = 1: dirMultiplier = -1;
            int newOffset = 90 * dirMultiplier;
            initialSetPoint = wrap180(initialSetPoint + newOffset);
            imu.setSetPoint(initialSetPoint);
        }
    }
    else if(taskStatus == TASK::OPEN_TURN){
        float error = imu.getError();
        if(abs(error) < 5){
            if(lapCount != 11){
                changeTask(TASK::FOLLOW_WALL);
                move.setTask(110,150,120,120,0,0);
                lapCount++;
            }
            else{
                changeTask(TASK::OPEN_ENDING);
                move.setTask(40,30,60,60,0,0);
            }
        }
        float theta = 0;
        if(direction == DIRECTIONS::COUNTERCLOCKWISE){
            theta = imu.getError() * 0.5;
        }
        else{
            theta = imu.getError() * 0.5; 
        }
        ackermann.setSteeringAngle(theta);
        move.driveAtPWM(-200);
    }
    else if(taskStatus == TASK::FOLLOW_WALL){
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
            changeTask(TASK::FOLLOW_UNTIL_EDGE);
        }
    }
    else if(taskStatus == TASK::FOLLOW_UNTIL_EDGE){
        steerByStanley(0.002f, 1.0f);

        frontDistance = validData.front;
        int innerWall = 0;
        if(direction == DIRECTIONS::COUNTERCLOCKWISE){
            innerWall = data.left;
        }
        else{
            innerWall = data.right;
        }
        if(frontDistance < 700 && frontDistance > 300 && innerWall > 3000){
            changeTask(TASK::OPEN_TURN);
            int dirMultiplier = 0;
            direction == DIRECTIONS::COUNTERCLOCKWISE? dirMultiplier = 1: dirMultiplier = -1;
            int newOffset = 90 * dirMultiplier;
            initialSetPoint = wrap180(initialSetPoint + newOffset);
            imu.setSetPoint(initialSetPoint);
            return;
        }
        else if( frontDistance > 600 || innerWall < 3000){
            move.driveAtSpeed(120,1,0.1,move.getCurrentSpeed());
            return;
        }
        move.driveAtPWM(200);
        delay(100);
        validData.front = 3000;
    }
    else if(taskStatus == TASK::OPEN_ENDING){
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
            changeTask(TASK::FINISHED);
        }        
    }
}


void Robot::selectTaskObstacles(){
    if(finish == true){
        move.driveAtPWM(0);
        return;
    }

    if(taskStatus == TASK::UNDEFINED){
        changeTask(TASK::EVADE_UNTIL_EDGE);
    }
    if(recoveryTurn){
        if(recoverySteering < 350){
            ackermann.setSteeringAngle(recoveryAngle);
            move.driveAtPWM(-OBSTACLE_DRIVE_PWM + 20);
            return;
        }
        recoveryTurn = false;
    }
    if(taskStatus == TASK::EVADE_UNTIL_EDGE){
        float imuError = imu.getError();

        bool cameraDataFresh = vision.receivedAtMs != 0 &&
            static_cast<uint32_t>(millis() - vision.receivedAtMs) <=
                CAMERA_DATA_TIMEOUT_MS;
        bool sentinelNotFound =
            vision.obstacleX == CAMERA_NOT_FOUND &&
            vision.obstacleY == CAMERA_NOT_FOUND;
        bool validObstacle = cameraDataFresh &&
            vision.obstacleDetected && !sentinelNotFound;
        float distanceSinceTurnMm =
            fabsf(move.controller.getDistanceMM());
        bool blueLineFilterActive =
            distanceSinceTurnMm >= 500.0f;
        bool blueLineVisible =
            cameraDataFresh && vision.blueLineDetected;

        if(blueLineVisible != blueLineLastSample){
            blueLineLastSample = blueLineVisible;
            blueLineStableAge = 0;
        }

        if(blueLineVisible && blueLineArmed &&
           blueLineFilterActive && blueLineStableAge >= 30){
            blueLineArmed = false;
            blueLineStableAge = 0;
            changeTask(TASK::APPROACH_BLUE_LINE);
            lapCount++;
            ui.buzzSound(4);
            return;
        }
        if(!blueLineVisible && !blueLineArmed &&
           blueLineStableAge >= 100 &&
           distanceSinceTurnMm >= BLUE_LINE_REARM_DISTANCE_MM){
            blueLineArmed = true;
        }
        bool shouldEvade = validObstacle;

        float steeringTarget = imuError * 1.0;
        Serial.print(shouldEvade);
        if(validData.left < 200){
            steeringTarget = -30;
        }
        else if(validData.right < 200){
            steeringTarget = 30;
        }
        else if(shouldEvade){
            float distanceToObstacle = sqrtf((pow(vision.obstacleX - 160,2))+(pow(vision.obstacleY,2)));
            
            float obstacleAngle =
                degrees(atan2f(vision.obstacleX - 160,vision.obstacleY));

            float evasionDirection =
                vision.obstacleColor == 2 ? 1.0f : -1.0f;
                steeringTarget = tc.tangentEvasion(
                    imuError,
                    evasionDirection,
                    obstacleAngle,
                    115.0f,
                    distanceToObstacle
                );
            if(distanceToObstacle < 80){
                recoveryTurn = true;
                recoverySteering = 0;
                recoveryAngle = -steeringTarget;
            }

        }
        else{
   
            tc.resetTangentEvasion(imuError);
        }

        ackermann.setSteeringAngle(steeringTarget);
        move.driveAtPWM(OBSTACLE_DRIVE_PWM);
    }

    else if(taskStatus == TASK::APPROACH_BLUE_LINE){
        ackermann.setSteeringAngle(imu.getError());
        move.driveAtPWM(OBSTACLE_DRIVE_PWM);

        if(validData.front < BLUE_LINE_FRONT_TARGET_MM){
            initialSetPoint = wrap180(initialSetPoint - 90.0f);
            imu.setSetPoint(initialSetPoint);
            blueLineReverseAge = 0;
            changeTask(TASK::REVERSE_AFTER_BLUE_LINE);
        }
    }

    else if(taskStatus == TASK::REVERSE_AFTER_BLUE_LINE){
        ackermann.setSteeringAngle(-imu.getError());
        move.driveAtPWM(-OBSTACLE_DRIVE_PWM);

        if(blueLineReverseAge >= BLUE_LINE_REVERSE_TIME_MS){
            move.controller.resetTicks();
            tc.resetTangentEvasion(imu.getError());
            blueLineStableAge = 0;
            blueLineLastSample = vision.blueLineDetected;
            changeTask(TASK::FORWARD_AFTER_REVERSE);
            move.driveAtPWM(OBSTACLE_DRIVE_PWM);
        }
    }

    else if(taskStatus == TASK::FORWARD_AFTER_REVERSE){
        ackermann.setSteeringAngle(imu.getError());
        move.driveAtPWM(OBSTACLE_DRIVE_PWM);

        const float forwardDistanceMm =
            fabsf(move.controller.getDistanceMM());
        
        if(forwardDistanceMm >= POST_REVERSE_STRAIGHT_DISTANCE_MM && lapCount != 12){
            tc.resetTangentEvasion(imu.getError());
            changeTask(TASK::EVADE_UNTIL_EDGE);
        }
        else if(forwardDistanceMm >= 1500){
            finish = true;
        }
    }

    else if(taskStatus == TASK::OBSTACLES_TURN){
        move.controller.resetTicks();
        float error = imu.getError();
        ackermann.setSteeringAngle(error);
        move.driveAtPWM(90);
        if(abs(error < 7.5)){
            taskStatus = TASK::EVADE_UNTIL_EDGE;
            ui.buzzSound(2);

        
        }

    }
}
