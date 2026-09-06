/**
 * @file Robot.cpp
 * @brief Robot instance to make it clearer.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 11/08/26
 */

 #include "Robot.h"


 Robot::Robot()
    :  tofs(SPI, CS_FRONT, CS_RIGHT, CS_LEFT, CS_BACK)
    {}

 void Robot::beginComms(){
    Serial.begin(PC_SERIAL_BAUD_RATE);
    IMU_SERIAL.begin(IMU_BAUD_RATE); // IMU
    Serial5.begin(XIAO_BAUD_RATE); // TX
    CAM_SERIAL.begin(CAM_SERIAL_BAUDRATE);

    delay(100);
    move.begin();
    delay(100);
    if(!imu.begin(IMU_SERIAL)){
        SCB_AIRCR = TEENSY_SOFT_REBOOT;
        while (true) {  }
    }
    delay(1000);
    if (!tofs.begin(TOFS_HZ)) {
        Serial.println("Error, pls reboot");
        SCB_AIRCR = TEENSY_SOFT_REBOOT; // soft reboot processor PENDING TESTING
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

void Robot::setImuSetPoint(){
    int dirMultiplier = 0;
    direction == DIRECTIONS::COUNTERCLOCKWISE? dirMultiplier = 1: dirMultiplier = -1;
    int newOffset = 90 * dirMultiplier;
    initialSetPoint = wrap180(initialSetPoint + newOffset);
    imu.setSetPoint(initialSetPoint);
}

void Robot::executeTaskOpen(){
    //verify if rutine is ended
    if(finish == true){
        return;
    }    
    if(taskStatus == TASK::UNDEFINED){
        setGoStraightToEdge();
    }
    else if(taskStatus == TASK::GO_STRAIGHT_TO_EDGE){
        executeGoStraightToEdge();
    }
    else if( taskStatus == TASK::GET_CLOSE_TO_EDGE){
        executeGetCloseToEdge();
    }
    else if(taskStatus == TASK::OPEN_TURN){
        executeOpenTurn();
    }
    else if(taskStatus == TASK::FOLLOW_WALL){
        executeFollowWall();
    }
    else if(taskStatus == TASK::FOLLOW_UNTIL_EDGE){
        executeFollowUntilEdge();
    }
    else if(taskStatus == TASK::OPEN_ENDING){
        executeOpenEnding();
    }
}

void Robot::executeGoStraightToEdge(){
    float error = imu.getError();
    float theta = error * 1.0f;
    ackermann.setSteeringAngle(theta);
    if(move.updateCM()){
        setGetCloseToEdge();
    }
}

void Robot::setGoStraightToEdge(){
    changeTask(TASK::GO_STRAIGHT_TO_EDGE);
    move.setTask(OPEN_INITIAL_STRAIGHT_PROFILE);
}


void Robot::executeGetCloseToEdge(){
    move.driveAtPWM(EDGING_PWM);
    if(validData.front < EDGING_TARGET_DISTANCE){
        decideDir();
        move.driveAtPWM(INITIAL_REVERSE_PWM);
        delay(INITIAL_REVERSE_TIME);
        setOpenTurn();
        setImuSetPoint();
    }
}

void Robot::setGetCloseToEdge(){
    changeTask(TASK::GET_CLOSE_TO_EDGE);
}


void Robot::executeFollowWall(){
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
        setFollowUntilEdge();
    }
}

void Robot::setFollowWall(){
    changeTask(TASK::FOLLOW_WALL);
    move.setTask(OPEN_FOLLOW_WALL_PROFILE);
}


void Robot::executeFollowUntilEdge(){
    steerByStanley(0.002f, 1.0f);

    frontDistance = validData.front;
    int innerWall = 0;
    if(direction == DIRECTIONS::COUNTERCLOCKWISE){
        innerWall = data.left;
    }
    else{
        innerWall = data.right;
    }
    if(frontDistance < TURN_MAX_DISTANCE_MM && frontDistance > TURN_MIN_DISTANCE_MM && innerWall > INNER_WALL_MIN_DISTANCE_TO_TURN_MM){
        setOpenTurn();
        setImuSetPoint();
        return;
    }
    else if( frontDistance > CRUISE_MIN_DISTANCE_MM || innerWall < INNER_WALL_MAX_DISTANCE_TO_CRUISE_MM){
        move.driveAtSpeed(OPEN_CRUISE_SPEED_CONTROL, move.getCurrentSpeed());
        return;
    }
    move.driveAtPWM(FOLLOW_WALL_RECOVERY_PWM);
    delay(100);
    validData.front = MAX_VALID_DISTANCE;
}

void Robot::setFollowUntilEdge(){
    changeTask(TASK::FOLLOW_UNTIL_EDGE);
}


void Robot::executeOpenTurn(){
    float error = imu.getError();
    if(abs(error) < OPEN_TURN_ERROR_THRESHOLD_DEG){
        if(lapCount != 11){
            setFollowWall();
            lapCount++;
        }
        else{
            setOpenEnding();
        }
    }
    float theta = 0;
    if(direction == DIRECTIONS::COUNTERCLOCKWISE){
        theta = imu.getError() * OPEN_TURN_STEERING_GAIN;
    }
    else{
        theta = imu.getError() * OPEN_TURN_STEERING_GAIN;
    }
    ackermann.setSteeringAngle(theta);
    move.driveAtPWM(OPEN_TURN_PWM);
}

void Robot::setOpenTurn(){
    changeTask(TASK::OPEN_TURN);
}


void Robot::executeOpenEnding(){
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

void Robot::setOpenEnding(){
    changeTask(TASK::OPEN_ENDING);
    move.setTask(OPEN_ENDING_PROFILE);
}

// Obstacle avoidance states

void Robot::executeTaskObstacles(){
    // Boolean to finish robot movement
    if(finish == true){
        move.driveAtPWM(0);
        return;
    }

    if(taskStatus == TASK::UNDEFINED){
        changeTask(TASK::EVADE_UNTIL_EDGE);
    }
    if(recoveryTurn){
        exceuteRecoveryTurn();
        if(recoveryTurn){
            return;
        }
    }
    if(taskStatus == TASK::EVADE_UNTIL_EDGE){
        executeEvadeUntilEdge();
    }
    else if(taskStatus == TASK::APPROACH_BLUE_LINE){
        executeApproachBlueLine();
    }

    else if(taskStatus == TASK::REVERSE_AFTER_BLUE_LINE){
        executeReverseAfterBlueLine();
    }

    else if(taskStatus == TASK::FORWARD_AFTER_REVERSE){
        executeForwardAfterReverse();
    }
}

void Robot::exceuteRecoveryTurn(){
    if(recoveryTurn){
        if(recoverySteering < OBSTACLES_RECOVERY_TIME_MS){ // add constant
            ackermann.setSteeringAngle(recoveryAngle);
            move.driveAtPWM(-OBSTACLES_PWM_DRIVE + 20);
        }
        else{
            recoveryTurn = false;
        }
    }
}

void Robot::setRecoveryTurn(float steeringTarget){
    recoveryTurn = true;
    recoverySteering = 0;
    recoveryAngle = -steeringTarget;
}

void Robot::executeEvadeUntilEdge(){
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
            distanceSinceTurnMm >= BLUE_LINE_REARM_DISTANCE_MM;
        bool blueLineVisible =
            cameraDataFresh && vision.blueLineDetected;

        if(blueLineVisible != blueLineLastSample){
            blueLineLastSample = blueLineVisible;
            blueLineStableAge = 0;
        }

        if(blueLineVisible && blueLineArmed &&
           blueLineFilterActive && blueLineStableAge >= 30){
            setApproachBlueLine();
            return;
        }
        if(!blueLineVisible && !blueLineArmed &&
           blueLineStableAge >= BLUE_LINE_STABLE_TIME_MS &&
           distanceSinceTurnMm >= BLUE_LINE_REARM_DISTANCE_MM){
            blueLineArmed = true;
        }
        bool shouldEvade = validObstacle;

        float steeringTarget = imu.getError() * NO_OBSTACLE_IMU_GAIN;

        Serial.print(shouldEvade);
        if(validData.left < SIDE_WALLS_ACTIVATION_DISTANCE_MM){
            steeringTarget = -SIDE_WALLS_STEERING_ANGLE_DEG;
        }
        else if(validData.right < SIDE_WALLS_ACTIVATION_DISTANCE_MM){
            steeringTarget = SIDE_WALLS_STEERING_ANGLE_DEG;
        }
        else if(shouldEvade){
            float distanceToObstacle = sqrtf((pow(vision.obstacleX - 160,2))+(pow(vision.obstacleY,2)));
            
            float obstacleAngle =
                degrees(atan2f(vision.obstacleX - VISION_WIDTH/2,vision.obstacleY));

            float evasionDirection =
                vision.obstacleColor == 2 ? 1.0f : -1.0f;
                steeringTarget = tc.tangentEvasion(
                    imuError,
                    evasionDirection,
                    obstacleAngle,
                    TAN_EVASION_SECURITY_RADIUS_MM,
                    distanceToObstacle
            );
            if(distanceToObstacle < OBSTACLE_CLOSE_RECOVERY_DISTANCE_MM){
                setRecoveryTurn(steeringTarget);
            }

        }
        else{
   
            tc.resetTangentEvasion(imuError);
        }

    ackermann.setSteeringAngle(steeringTarget);
    move.driveAtPWM(OBSTACLE_DRIVE_PWM);
}

void Robot::setEvadeUntilEdge(){
    tc.resetTangentEvasion(imu.getError());
    changeTask(TASK::EVADE_UNTIL_EDGE);
}

void Robot::executeApproachBlueLine(){
    ackermann.setSteeringAngle(imu.getError());
    move.driveAtPWM(OBSTACLE_DRIVE_PWM);

    if(validData.front < BLUE_LINE_FRONT_TARGET_MM){
        setReverseAfterBlueLine();
    }
}

void Robot::setApproachBlueLine(){
    blueLineArmed = false;
    blueLineStableAge = 0;
    lapCount++;
    ui.buzzSound(4);
    changeTask(TASK::APPROACH_BLUE_LINE);
}

void Robot::executeReverseAfterBlueLine(){
    ackermann.setSteeringAngle(-imu.getError());
    move.driveAtPWM(-OBSTACLE_DRIVE_PWM);

    if(blueLineReverseAge >= BLUE_LINE_REVERSE_TIME_MS){
        setForwardAfterReverse();
    }
}

void Robot::setReverseAfterBlueLine(){
    initialSetPoint = wrap180(initialSetPoint - 90.0f);
    imu.setSetPoint(initialSetPoint);
    blueLineReverseAge = 0;
    changeTask(TASK::REVERSE_AFTER_BLUE_LINE);
}

void Robot::executeForwardAfterReverse(){
    ackermann.setSteeringAngle(imu.getError());
    move.driveAtPWM(OBSTACLE_DRIVE_PWM);

    const float forwardDistanceMm =
        fabsf(move.controller.getDistanceMM());

    if(forwardDistanceMm >= POST_REVERSE_STRAIGHT_DISTANCE_MM && lapCount != 12){
        setEvadeUntilEdge();
    }
    else if(forwardDistanceMm >= PARKING_DISTANCE_MM){
        finish = true;
    }
}

void Robot::setForwardAfterReverse(){
    move.controller.resetTicks();
    tc.resetTangentEvasion(imu.getError());
    blueLineStableAge = 0;
    blueLineLastSample = vision.blueLineDetected;
    changeTask(TASK::FORWARD_AFTER_REVERSE);
    move.driveAtPWM(OBSTACLE_DRIVE_PWM);
}
