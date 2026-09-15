// Run with: python3 tests/check_obstacle_evasion_integration.py
#include "DataTypes.h"
#include "ObstacleEvasion.h"
#include "Utils.h"
#include <assert.h>
#include <initializer_list>
#include <stdio.h>

uint32_t testNow = 1000;
struct CameraPort { int last = 0; void write(int value) { last = value; } } cameraPort;
#define CAM_SERIAL cameraPort

// Startup methods and constants are copied directly from the current firmware by the runner.
struct Robot {
    #include "robot_startup_constants.inc"
    struct IMU {
        float error = 7, setPoint = 0;
        float getError() { return error; }
        void setSetPoint(float value) { setPoint = value; }
    } imu;
    struct Ackermann { float angle = 0; void setSteeringAngle(float value) { angle = value; } } ackermann;
    struct Move {
        struct Controller { int resets = 0; void resetTicks() { ++resets; } } controller;
        int pwm = 0;
        void driveAtPWM(int value) { pwm = value; }
    } move;
    struct TC { void resetTangentEvasion(float) {} } tc;
    struct UI { void buzzSound(int) {} } ui;
    SensorData validData;
    OpenMVData vision;
    TASK taskStatus = TASK::UNDEFINED;
    DIRECTIONS direction = DIRECTIONS::COUNTERCLOCKWISE;
    DIRECTIONS parkingDirectionCandidate = DIRECTIONS::COUNTERCLOCKWISE;
    uint32_t parkingDirectionLastFrameMs = 0;
    uint8_t parkingDirectionSamples = 0;
    bool finish = false, recoveryTurn = false, obstacleDirectionKnown = false;
    bool parkingSearchStopped = false, blueLineArmed = true;
    int lapCount = 0, evasionCalls = 0;
    float initialSetPoint = 0;
    elapsedMillis roiChangeTimer, parkingSearchAge, blueLineReverseAge;
    elapsedMillis blueLineStableAge, blueLineForwardTimeOut;
    Robot() { validData.back = 500; }
    void changeTask(TASK next) { taskStatus = next; validData.front = 3001; }
    void exceuteRecoveryTurn() {}
    void executeEvadeUntilEdge() { ++evasionCalls; move.driveAtPWM(OBSTACLE_DRIVE_PWM); }
    void executeApproachBlueLine() {}
    void executeReverseAfterBlueLine() {}
    void executeForwardAfterReverse() {}
    void executeObstaclesEnding() {}
    void decideDir() { assert(false && "Obstacle turns must not reselect direction from ToFs"); }
    void executeTaskObstacles();
    void decideDirObstacles();
    void setEvadeUntilEdge();
    void setApproachBlueLine();
    void setReverseAfterBlueLine();
    void setImuSetPoint();
};

#include "robot_startup_under_test.inc"

static void start(Robot& robot, uint32_t now = 1000) {
    testNow = now;
    robot.vision.receivedAtMs = now;
    robot.vision.wallDetected = true; // A pre-start detection must not count.
    robot.vision.wallX = 140; robot.vision.wallY = 100;
    robot.executeTaskObstacles();
    assert(robot.taskStatus == Robot::TASK::DECIDE_DIR_OBSTACLES);
    assert(robot.parkingDirectionSamples == 0 && !robot.obstacleDirectionKnown);
    assert(robot.move.pwm == Robot::PARKING_SEARCH_REVERSE_PWM);
    assert(robot.ackermann.angle == -robot.imu.error && robot.evasionCalls == 0);
}

static void frame(Robot& robot, uint32_t now, int x, int y = 100, bool detected = true) {
    testNow = now;
    robot.vision.receivedAtMs = now;
    robot.vision.wallDetected = detected;
    robot.vision.wallX = x; robot.vision.wallY = y;
    robot.executeTaskObstacles();
}

int main() {
    for (int x : {140, 180}) {
        Robot robot; start(robot);
        frame(robot, 1067, x); frame(robot, 1134, x);
        assert(!robot.obstacleDirectionKnown);
        frame(robot, 1201, x);
        const auto expected = x < 160 ? DIRECTIONS::COUNTERCLOCKWISE : DIRECTIONS::CLOCKWISE;
        assert(robot.direction == expected && robot.obstacleDirectionKnown);
        assert(robot.taskStatus == Robot::TASK::EVADE_UNTIL_EDGE && robot.move.pwm == 0);
        assert(robot.move.controller.resets == 1 && uint32_t(robot.roiChangeTimer) == 0);
        frame(robot, 1268, 320 - x); // Later detections do not change the chosen direction.
        assert(robot.direction == expected && robot.evasionCalls == 1);
        assert(robot.move.pwm == Robot::OBSTACLE_DRIVE_PWM && cameraPort.last == 2);
        robot.validData.left = x < 160 ? 100 : 3001;
        robot.validData.right = x < 160 ? 3001 : 100;
        robot.setApproachBlueLine(); robot.setReverseAfterBlueLine();
        assert(robot.direction == expected && robot.lapCount == 1);
        assert(robot.imu.setPoint == (x < 160 ? 90 : -90));
    }
    {
        Robot robot; start(robot); frame(robot, 1067, 140);
        for (testNow = 1068; testNow < 1134; ++testNow) robot.executeTaskObstacles();
        assert(robot.parkingDirectionSamples == 1); // Reusing a frame cannot confirm it.
        frame(robot, 1134, 180); assert(robot.parkingDirectionSamples == 1);
        frame(robot, 1201, 140); assert(robot.parkingDirectionSamples == 1);
        frame(robot, 1268, 140); frame(robot, 1335, 140);
        assert(robot.obstacleDirectionKnown);
    }
    {
        Robot robot; start(robot);
        for (int x : {155, 160, 165}) {
            frame(robot, testNow + 67, 140);
            frame(robot, testNow + 67, x);
            assert(robot.parkingDirectionSamples == 0);
        }
        frame(robot, testNow + 67, 140, 100, false);
        frame(robot, testNow + 67, 250, 250);
        frame(robot, testNow + 67, 320, 100);
        assert(!robot.obstacleDirectionKnown && robot.parkingDirectionSamples == 0);
    }
    {
        Robot robot; start(robot); frame(robot, 1067, 140); frame(robot, 1134, 140);
        frame(robot, 2200, 140); // A pause also resets the streak if no loops ran during it.
        assert(robot.parkingDirectionSamples == 1);
        testNow = 2800; robot.executeTaskObstacles();
        assert(robot.parkingDirectionSamples == 0 && !robot.obstacleDirectionKnown);
    }
    {
        Robot robot; start(robot);
        robot.validData.back = 99;
        frame(robot, 1067, 140);
        assert(robot.move.pwm == 0 && robot.parkingSearchStopped);
        robot.validData.back = 3001; // An invalid rear reading must not restart reverse.
        testNow = 1080; robot.executeTaskObstacles(); assert(robot.move.pwm == 0);
        frame(robot, 1134, 140); frame(robot, 1201, 140);
        assert(robot.obstacleDirectionKnown && robot.taskStatus == Robot::TASK::EVADE_UNTIL_EDGE);
    }
    {
        Robot robot; start(robot);
        testNow = 5999; robot.executeTaskObstacles(); assert(robot.move.pwm == 70);
        testNow = 6000; robot.executeTaskObstacles();
        assert(robot.move.pwm == 0 && !robot.obstacleDirectionKnown && robot.parkingSearchStopped);
        frame(robot, 6067, 180); frame(robot, 6134, 180); frame(robot, 6201, 180);
        assert(robot.obstacleDirectionKnown && robot.direction == DIRECTIONS::CLOCKWISE);
    }
    {
        const uint32_t initial = UINT32_MAX - 100;
        Robot robot; start(robot, initial);
        frame(robot, initial + 30, 180); frame(robot, initial + 60, 180);
        frame(robot, initial + 130, 180);
        assert(robot.obstacleDirectionKnown && robot.direction == DIRECTIONS::CLOCKWISE);
    }
    puts("Parking startup: both sides, fresh-frame confirmation, first turn, rear stop, timeout and clock wrap passed");
}
