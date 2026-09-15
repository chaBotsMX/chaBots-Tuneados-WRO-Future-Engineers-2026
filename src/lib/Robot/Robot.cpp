/**
 * @file Robot.cpp
 * @brief Robot instance to make it clearer.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 11/08/26
 */

 #include "Robot.h"
 #include "ObstacleEvasion.h"


 Robot::Robot()
    :  tofs(SPI, CS_FRONT, CS_RIGHT, CS_LEFT, CS_BACK)
    {}

 void Robot::beginComms(){
    Serial.begin(PC_SERIAL_BAUD_RATE);
    ui.printDebugDisplayStatus();
    IMU_SERIAL.addMemoryForRead(bnoRxBuffer, sizeof(bnoRxBuffer));
    Serial5.begin(XIAO_BAUD_RATE); // TX
    CAM_SERIAL.begin(CAM_SERIAL_BAUDRATE);

    delay(100);
    move.begin();
    move.driveAtPWM(0);
    delay(300);
    Serial.println("[BOOT] IMU");
    ui.showStartupStatus("INIT IMU");
    if(!imu.begin(IMU_SERIAL)){
        stopOnStartupError("ERROR IMU");
    }
    delay(300);
    delay(1000);
    Serial.println("[BOOT] TOF 8x8 / 15 Hz");
    ui.neoColor(255, 128, 0);
    ui.showStartupStatus("INIT TOF");
    if (!tofs.begin(TOFS_HZ)) {
        stopOnStartupError("ERROR TOF");
    }
    Serial.println("All TOFs ready");
    ui.neoColor(0, 255, 0);
    delay(1000);
 }

[[noreturn]] void Robot::stopOnStartupError(const char* message){
    move.driveAtPWM(0);
    Serial.println(message);
    ui.neoColor(255, 0, 0);
    ui.buzzSound(0, 500, 300);
    while (true) {
        ui.showDebug(message, INVALID_DISTANCE, INVALID_DISTANCE,
                     INVALID_DISTANCE, NAN, NAN);
        delay(1); // Fault state only; never enter the driving loop after a failed boot.
    }
}

void Robot::updateCam(){
    camera.update(CAM_SERIAL,vision);
}

void Robot::updateCamOpen(bool rememberLine){
    camera.updateOpen(CAM_SERIAL,vision);
    if(rememberLine && lineId == 0 && vision.lineId != 3 && vision.lineDetected){
        lineId = vision.lineId;        
    }
}

bool Robot::updateSensors(){
    tofs.update();

    bool hasFreshData = false;
    hasFreshData |= updateSide(TOF4Walls::FRONT, data.front, validData.front, frontDataAge);
    hasFreshData |= updateSide(TOF4Walls::BACK, data.back, validData.back, backDataAge);
    const bool leftFresh = updateSide(TOF4Walls::LEFT, data.left, validData.left, leftDataAge);
    const bool rightFresh = updateSide(TOF4Walls::RIGHT, data.right, validData.right, rightDataAge);
    hasFreshData |= leftFresh | rightFresh;

    invalidateStaleSensorData();

    if (taskStatus == TASK::GO_STRAIGHT_TO_EDGE || taskStatus == TASK::GET_CLOSE_TO_EDGE) {
        const uint32_t nowMs = millis();
        if (leftFresh) {
            openDirectionMonitor.observe(OpenDirectionMonitor::Side::LEFT,
                validData.left, validData.left < MAX_VALID_DISTANCE, nowMs);
        }
        if (rightFresh) {
            openDirectionMonitor.observe(OpenDirectionMonitor::Side::RIGHT,
                validData.right, validData.right < MAX_VALID_DISTANCE, nowMs);
        }
    }

    refreshDebugDisplay(); // HCI limits frames to 100 ms and sends one small packet.

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

    if(filteredDistance != INVALID_DISTANCE){
        if(side == TOF4Walls::LEFT){
            lastUsefulLeftDistance = filteredDistance;
        }
        else if(side == TOF4Walls::RIGHT){
            lastUsefulRightDistance = filteredDistance;
        }
    }
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
    const char* state = finish ? "FINISHED" :
                        recoveryTurn ? "RECOVERY" : taskName(taskStatus);
    // Only complete, valid camera packets update receivedAtMs.
    const bool cameraUartOk = vision.receivedAtMs != 0 &&
        static_cast<uint32_t>(millis() - vision.receivedAtMs) <= CAMERA_DATA_TIMEOUT_MS;
    ui.showDebug(state,
                 data.front, data.left, data.right,
                 imu.getYaw(), imu.setPoint, cameraUartOk);
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
    this->ui.begin();
    this->beginComms();
    delay(1000);
    this->ackermann.begin();
    this->beginData();

    while (this->ui.buttonRead() == false) {
    if(this->updateSensors() == true){
    this->ui.neoColor(0,255,0);
    }
    }
    this->ui.neoColor(0,0,255);
    this->ui.buzzSound(1);
    this->imu.setSetPoint(0); 
}

void Robot::decideDir(){
    if (taskStatus == TASK::GET_CLOSE_TO_EDGE) {
        const auto gap = openDirectionMonitor.gap(millis());
        if (gap == OpenDirectionMonitor::Gap::LEFT) {
            direction = DIRECTIONS::COUNTERCLOCKWISE;
            return;
        }
        if (gap == OpenDirectionMonitor::Gap::RIGHT) {
            direction = DIRECTIONS::CLOCKWISE;
            return;
        }
    }

    uint16_t leftDistance = validData.left;
    uint16_t rightDistance = validData.right;

    bool leftIsValid = leftDistance < 1000;
    bool rightIsValid = rightDistance < 1000;

    if(!leftIsValid && !rightIsValid){
        leftDistance = lastUsefulLeftDistance;
        rightDistance = lastUsefulRightDistance;

        // If neither side has ever been measured, keep the configured default.
        if(leftDistance > MAX_VALID_DISTANCE &&
           rightDistance > MAX_VALID_DISTANCE){
            return;
        }
    }

    if(rightDistance > leftDistance){
        direction = DIRECTIONS::CLOCKWISE;
    }
    else{
        direction = DIRECTIONS::COUNTERCLOCKWISE;
    }/*
    if(lineId == 2){
        direction = DIRECTIONS::CLOCKWISE;
    }
    else{
        direction = DIRECTIONS::COUNTERCLOCKWISE;
    }*/
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
        case TASK::DECIDE_DIR_OBSTACLES:return "FIND_PARKING";
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
    openDirectionMonitor.begin(millis());
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
        float stanleyTheta = tc.stanley(wallDistance - validData.left,-imu.getError(),float(move.getCurrentSpeed()), 0.005f, 1.0f);
        ackermann.setSteeringAngle(-stanleyTheta);
        Serial.print("stanleyTheta: ");
        Serial.println(stanleyTheta);
    }
    else{
        float stanleyTheta = tc.stanley(wallDistance - validData.right,imu.getError(),float(move.getCurrentSpeed()), 0.005f, 1.0f);
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
    if (move.getCurrentSpeed() >= 0) {
        steerByStanley(0.002f, 1.0f);
    }

    frontDistance = validData.front;
    int innerWall = 0;
    if(direction == DIRECTIONS::COUNTERCLOCKWISE){
        innerWall = data.left;
    }
    else{
        innerWall = data.right;
    }
    if(frontDistance < 250 && innerWall > 2000){
        move.driveAtPWM(FOLLOW_WALL_RECOVERY_PWM);
        delay(500);
        move.controller.brake();
        delay(100);
        move.driveAtPWM(0);
        delay(100);
        validData.front = MAX_VALID_DISTANCE;
        setOpenTurn();
        setImuSetPoint();
        return;
    }
    else if(frontDistance < TURN_MAX_DISTANCE_MM && frontDistance > TURN_MIN_DISTANCE_MM && innerWall > INNER_WALL_MIN_DISTANCE_TO_TURN_MM){
        setOpenTurn();
        setImuSetPoint();
        return;
    }
    else if( frontDistance > CRUISE_MIN_DISTANCE_MM || innerWall < INNER_WALL_MAX_DISTANCE_TO_CRUISE_MM){
        move.driveAtSpeed(OPEN_CRUISE_SPEED_CONTROL, move.getCurrentSpeed());
        return;
    }
    move.driveAtPWM(FOLLOW_WALL_RECOVERY_PWM);
    delay(500);
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

    roiChangeTimer > 212 ? CAM_SERIAL.write(1) : CAM_SERIAL.write(2);

    if(taskStatus == TASK::UNDEFINED){
        obstacleDirectionKnown = false;
        parkingDirectionSamples = 0;
        // Require new packets after starting, not an image saved before the button.
        parkingDirectionLastFrameMs = vision.receivedAtMs;
        parkingSearchAge = 0;
        changeTask(TASK::DECIDE_DIR_OBSTACLES);
    }
    if(taskStatus == TASK::DECIDE_DIR_OBSTACLES){
        decideDirObstacles();
        return;
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
    else if(taskStatus == TASK::OBSTACLES_ENDING){
        executeObstaclesEnding();
    }
}

void Robot::decideDirObstacles(){
    if (obstacleDirectionKnown) return;

    const bool cameraDataFresh = vision.receivedAtMs != 0 &&
        static_cast<uint32_t>(millis() - vision.receivedAtMs) <= CAMERA_DATA_TIMEOUT_MS;
    if (!cameraDataFresh) {
        parkingDirectionSamples = 0;
    }
    else if (vision.receivedAtMs != parkingDirectionLastFrameMs) {
        parkingDirectionLastFrameMs = vision.receivedAtMs;
        const int imageOffset = int(vision.wallX) - VISION_WIDTH / 2;
        if (!vision.wallDetected ||
            !ObstacleEvasion::validPosition(vision.wallX, vision.wallY) ||
            abs(imageOffset) <= PARKING_DIRECTION_CENTER_MARGIN_PX) {
            parkingDirectionSamples = 0;
        }
        else {
            // Camera rotated 180 degrees: image-left means parking on robot-right.
            const DIRECTIONS candidate = imageOffset < 0
                ? DIRECTIONS::COUNTERCLOCKWISE : DIRECTIONS::CLOCKWISE;
            if (parkingDirectionSamples == 0 || candidate != parkingDirectionCandidate) {
                parkingDirectionCandidate = candidate;
                parkingDirectionSamples = 1;
            }
            else if (parkingDirectionSamples < PARKING_DIRECTION_CONFIRM_SAMPLES) {
                ++parkingDirectionSamples;
            }
            if (parkingDirectionSamples >= PARKING_DIRECTION_CONFIRM_SAMPLES) {
                direction = parkingDirectionCandidate;
                obstacleDirectionKnown = true;
                move.driveAtPWM(0);
                move.controller.resetTicks();
                roiChangeTimer = 0;
                setEvadeUntilEdge();
                return;
            }
        }
    }

    // Same reverse heading correction as the existing reverse maneuver.
    ackermann.setSteeringAngle(-imu.getError());
    const bool rearWallClose = validData.back < PARKING_SEARCH_REAR_STOP_MM;
    const bool searchTimedOut = parkingSearchAge >= PARKING_SEARCH_TIMEOUT_MS;
    move.driveAtPWM(rearWallClose || searchTimedOut ? 0 : PARKING_SEARCH_REVERSE_PWM);
}

void Robot::exceuteRecoveryTurn(){
    if(recoveryTurn){
        if(recoverySteering < OBSTACLES_RECOVERY_TIME_MS){ // add constant
            ackermann.setSteeringAngle(recoveryAngle);
            move.driveAtPWM(-OBSTACLES_PWM_DRIVE + 20);
        }
        else{

            elapsedMillis WTF;
            WTF = 0;
            while(WTF < 500){
                ackermann.setSteeringAngle(-recoveryAngle);
                move.driveAtPWM(0);
            }
            CAM_SERIAL.flush();
            delay(100); 
            recoveryTurn = false;
        }
    }
}

void Robot::setRecoveryTurn(float steeringTarget){
    elapsedMillis WTF;
    recoveryTurn = true;
    recoveryAngle = -steeringTarget;
    WTF = 0;
    while(WTF < 1000){
        ackermann.setSteeringAngle(recoveryAngle);
        //move.driveAtPWM(0);
        move.controller.brake();
    }
    recoverySteering = 0;
}

void Robot::executeEvadeUntilEdge(){
        float imuError = imu.getError();

        bool cameraDataFresh = vision.receivedAtMs != 0 &&
            static_cast<uint32_t>(millis() - vision.receivedAtMs) <=
                CAMERA_DATA_TIMEOUT_MS;
        bool validObstacle = cameraDataFresh &&
            vision.obstacleDetected &&
            ObstacleEvasion::validPosition(vision.obstacleX, vision.obstacleY) &&
            ObstacleEvasion::validColor(vision.obstacleColor);
        float distanceSinceTurnMm =
            fabsf(move.controller.getDistanceMM());
        bool blueLineFilterActive = lapCount > 0 ? distanceSinceTurnMm >= BLUE_LINE_REARM_DISTANCE_MM : true;
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
        const bool correctSide = validObstacle && ObstacleEvasion::canIgnoreObstacle(
            vision.obstacleX, vision.obstacleY, vision.obstacleColor,
            imuError, TAN_EVASION_SECURITY_RADIUS_PX);
        const bool shouldEvade = validObstacle && !correctSide;

        const float parkingSteering = cameraDataFresh && vision.wallDetected
            ? ObstacleEvasion::parkingSteering(vision.wallX, vision.wallY,
                direction == DIRECTIONS::CLOCKWISE, obstacleDirectionKnown)
            : 0.0f;
        float steeringTarget = imuError * NO_OBSTACLE_IMU_GAIN;

        Serial.print(shouldEvade);
        if(parkingSteering != 0.0f){
            steeringTarget = parkingSteering;
        }
        else if(validData.left < SIDE_WALLS_ACTIVATION_DISTANCE_MM){
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
                vision.obstacleColor == GREEN_OBSTACLE ? 1.0f : -1.0f;
                steeringTarget = tc.tangentEvasion(
                    imuError,
                    evasionDirection,
                    obstacleAngle,
                    TAN_EVASION_SECURITY_RADIUS_PX,
                    distanceToObstacle
            );
            if(distanceToObstacle < OBSTACLE_CLOSE_RECOVERY_DISTANCE_PX){
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
    float steeringTarget = imu.getError();
    move.driveAtPWM(OBSTACLE_DRIVE_PWM);
    if(validData.left < 300){
        steeringTarget = -SIDE_WALLS_STEERING_ANGLE_DEG;
    }
    else if(validData.right < 300){
        steeringTarget = SIDE_WALLS_STEERING_ANGLE_DEG;
    }
    if(validData.front < BLUE_LINE_FRONT_TARGET_MM || blueLineForwardTimeOut > 3500){
        setReverseAfterBlueLine();
    }
    ackermann.setSteeringAngle(steeringTarget);
}

void Robot::setApproachBlueLine(){
    blueLineArmed = false;
    blueLineStableAge = 0;
    lapCount++;
    ui.buzzSound(4);
    blueLineForwardTimeOut = 0;
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
    setImuSetPoint();
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
        roiChangeTimer = 0;

    }
    else if(forwardDistanceMm >= PARKING_DISTANCE_MM){
        setObstaclesEnding();
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

void Robot::executeObstaclesEnding(){
    if(imu.getError() < 0.1f && imu.getError() > -0.1f){
        move.driveAtPWM(0);
        delay(1500);
        move.driveAtPWM(70);
        delay(3000);
        finish = true;

    }
    else{
        ackermann.setSteeringAngle(imu.getError() * 2.0);
        move.driveAtPWM(-70);
    }
}

void Robot::setObstaclesEnding(){
    setImuSetPoint();
    changeTask(TASK::OBSTACLES_ENDING);
}