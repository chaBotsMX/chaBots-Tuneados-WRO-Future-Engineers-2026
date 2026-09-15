// Run with: python3 tests/check_obstacle_evasion_integration.py
#include "DataTypes.h"
#include "ObstacleEvasion.h"
#include "OpenDirectionMonitor.h"
#include "TrajectoryController.h"

#include <assert.h>
#include <initializer_list>
#include <stdio.h>

uint32_t testNow = 1000;
struct SerialStub { template<class T> void print(T) {} } Serial;

// Only hardware and unrelated states are replaced. The evasion method below is
// extracted from Robot.cpp, and TrajectoryController.cpp is compiled unchanged.
struct Robot {
    struct IMU { float error = 0; float getError() { return error; } } imu;
    struct Ackermann { float angle = 0; void setSteeringAngle(float value) { angle = value; } } ackermann;
    struct Move {
        struct Controller { float getDistanceMM() { return 2000; } } controller;
        int pwm = 0;
        void driveAtPWM(int value) { pwm = value; }
    } move;
    TrajectoryController tc;
    OpenDirectionMonitor openDirectionMonitor;
    OpenMVData vision;
    SensorData validData;
    DIRECTIONS direction = DIRECTIONS::COUNTERCLOCKWISE;
    static constexpr uint8_t GREEN_OBSTACLE = 2;
    static constexpr uint32_t CAMERA_DATA_TIMEOUT_MS = 500;
    static constexpr int OBSTACLE_DRIVE_PWM = -100;
    int lapCount = 1;
    bool obstacleDirectionKnown = true;
    bool blueLineArmed = true, blueLineLastSample = false, approach = false, recovery = false;
    elapsedMillis blueLineStableAge;
    Robot() {
        validData.left = validData.right = 500;
        vision.receivedAtMs = testNow;
    }
    void setApproachBlueLine() { approach = true; ++lapCount; }
    void setRecoveryTurn(float) { recovery = true; }
    void executeEvadeUntilEdge();
};

#include "robot_evade_under_test.inc"

static void obstacle(Robot& robot, int x, int y, int color) {
    robot.vision.obstacleDetected = true;
    robot.vision.obstacleColor = color;
    robot.vision.obstacleX = x;
    robot.vision.obstacleY = y;
    robot.vision.receivedAtMs = testNow;
}

static bool close(float a, float b) { return fabsf(a - b) < 0.001f; }

int main() {
    for (auto direction : {DIRECTIONS::CLOCKWISE, DIRECTIONS::COUNTERCLOCKWISE}) {
        // Color selects the passing side relative to the robot in either course direction.
        Robot red, green;
        red.direction = green.direction = direction;
        obstacle(red, 160, 100, 1); obstacle(green, 160, 100, 2);
        red.executeEvadeUntilEdge(); green.executeEvadeUntilEdge();
        assert(red.ackermann.angle < 0 && green.ackermann.angle > 0);
        assert(close(red.ackermann.angle, -green.ackermann.angle));

        // Exceeding 10 degrees on the correct side must not skip a close block.
        obstacle(red, 210, 80, 1); obstacle(green, 110, 80, 2);
        testNow += 67;
        red.executeEvadeUntilEdge(); green.executeEvadeUntilEdge();
        assert(red.ackermann.angle < 0 && green.ackermann.angle > 0);

        // Once clearance is sufficient, normal heading control returns and resets PD memory.
        obstacle(red, 290, 80, 1); obstacle(green, 30, 80, 2);
        testNow += 67;
        red.executeEvadeUntilEdge(); green.executeEvadeUntilEdge();
        assert(red.ackermann.angle == 0 && green.ackermann.angle == 0);
        Robot freshRed;
        obstacle(red, 160, 100, 1); obstacle(freshRed, 160, 100, 1);
        testNow += 67;
        red.executeEvadeUntilEdge(); freshRed.executeEvadeUntilEdge();
        assert(close(red.ackermann.angle, freshRed.ackermann.angle));

        Robot parking;
        parking.direction = direction;
        parking.vision.wallDetected = true;
        parking.vision.wallY = 100;
        parking.vision.wallX = direction == DIRECTIONS::CLOCKWISE ? 180 : 140;
        parking.executeEvadeUntilEdge();
        assert(parking.ackermann.angle == (direction == DIRECTIONS::CLOCKWISE ? -15 : 15));
        assert(parking.move.pwm == Robot::OBSTACLE_DRIVE_PWM);

        // Sentinel and stale parking coordinates must not leave a steering command active.
        parking.imu.error = 2;
        parking.vision.wallX = parking.vision.wallY = 250;
        parking.executeEvadeUntilEdge();
        assert(parking.ackermann.angle == 2 * NO_OBSTACLE_IMU_GAIN);
        parking.vision.wallX = 160; parking.vision.wallY = 100;
        parking.vision.receivedAtMs = testNow - 501;
        parking.executeEvadeUntilEdge();
        assert(parking.ackermann.angle == 2 * NO_OBSTACLE_IMU_GAIN);

        // Before the first turn, visual side overrides either configured default direction.
        for (int x : {140, 180}) {
            Robot startup;
            startup.direction = direction; startup.lapCount = 0;
            startup.obstacleDirectionKnown = false;
            startup.vision.wallDetected = true;
            startup.vision.wallX = x; startup.vision.wallY = 100;
            startup.executeEvadeUntilEdge();
            assert(startup.ackermann.angle == (x < 160 ? 15 : -15));
        }

        // Invalid colors and expired detections return to heading control.
        for (int color : {0, 3}) {
            Robot invalid;
            invalid.direction = direction; invalid.imu.error = -2;
            obstacle(invalid, 160, 100, color);
            invalid.executeEvadeUntilEdge();
            assert(invalid.ackermann.angle == -2 * NO_OBSTACLE_IMU_GAIN);
        }
        Robot stale;
        stale.direction = direction; stale.imu.error = 2;
        obstacle(stale, 160, 100, 1); stale.vision.receivedAtMs = testNow - 501;
        stale.executeEvadeUntilEdge();
        assert(stale.ackermann.angle == 2 * NO_OBSTACLE_IMU_GAIN);

        // Existing side-wall protection and blue-line transition remain active in both directions.
        Robot left, right, line;
        left.direction = right.direction = line.direction = direction;
        left.validData.left = right.validData.right = 50;
        left.executeEvadeUntilEdge(); right.executeEvadeUntilEdge();
        assert(left.ackermann.angle == -SIDE_WALLS_STEERING_ANGLE_DEG);
        assert(right.ackermann.angle == SIDE_WALLS_STEERING_ANGLE_DEG);
        line.vision.blueLineDetected = line.blueLineLastSample = true;
        line.blueLineStableAge = 40;
        line.executeEvadeUntilEdge();
        assert(line.approach);
    }

    // Stateful PD outputs mirror too, including transitions into/out of the ignore condition.
    unsigned comparisons = 0;
    Robot clockwise, counterclockwise;
    clockwise.direction = DIRECTIONS::CLOCKWISE;
    for (int x : {160, 180, 210, 280, 290, 160}) {
        for (float error : {-10.0f, 0.0f, 10.0f}) {
            testNow += 67;
            obstacle(clockwise, x, 100, 1);
            obstacle(counterclockwise, 320 - x, 100, 2);
            clockwise.imu.error = error; counterclockwise.imu.error = -error;
            clockwise.executeEvadeUntilEdge(); counterclockwise.executeEvadeUntilEdge();
            assert(close(clockwise.ackermann.angle, -counterclockwise.ackermann.angle));
            assert(clockwise.move.pwm == counterclockwise.move.pwm);
            ++comparisons;
        }
    }
    printf("Robot integration: both directions, parking, stale inputs and %u mirrored PD steps passed\n", comparisons);
}
