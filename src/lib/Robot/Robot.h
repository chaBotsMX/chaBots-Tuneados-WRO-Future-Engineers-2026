 /**
 * @file Robot.h
 * @brief Robot instance to make it clearer.
 * 
 * @author Roy Barron / chaBotsMX
 * @date 11/08/26
 */

#ifndef ROBOT_H
#define ROBOT_H

#include <Arduino.h>
#include <HardwareSerial.h>
#include "AckermannController.h"
#include "HCI.h"
#include <Move.h>
#include "IMU.h"
#include "TrajectoryController.h"
#include "ControlValues.h"
#include "DataTypes.h"
#include "TOF4Walls.h"
#include <SPI.h>
#include <elapsedMillis.h>
#include "cameraReceiver.h"

#define IMU_BAUD_RATE 115200
#define XIAO_BAUD_RATE 2000000
#define PC_SERIAL_BAUD_RATE 115200


#define IMU_SERIAL Serial4

#define CAM_SERIAL Serial3
#define CAM_SERIAL_BAUDRATE 115200

#define CS_FRONT 7
#define CS_RIGHT 6
#define CS_LEFT 4
#define CS_BACK 5

#define TOFS_HZ 60

#define  TEENSY_SOFT_REBOOT 0x05FA0004

// High-level facade: each loop updates sensors and runs exactly one state
// machine (open round or obstacles).
class Robot {
public:
    Move move;
    SensorData data;
    SensorData validData;
    HCI ui;
    AckermannController ackermann;
    IMU imu;
    TrajectoryController tc;
    TOF4Walls tofs;
    cameraReceiver camera;

    Robot();
    void updateCam();
    void beginComms();

    // Updates ToF data and returns true when at least one side has a new reading.
    bool updateSensors();
    void printData();


    void steerByStanley(float stanleyWallGain, float stanleyHeadingGain);

    // Advances the IMU reference by one quarter turn in the selected direction.
    void setImuSetPoint();

    void beginData();

    void begin();

    // Runs one non-blocking step of the open round.
    void executeTaskOpen();

    // Runs one non-blocking step of evasion, blue-line, and parking logic.
    void executeTaskObstacles();

    void decideDir();




    int frontDistance = 0;
    float initialSetPoint = 0.0f;
    
    DIRECTIONS direction = DIRECTIONS::COUNTERCLOCKWISE;

    // Open round states

    void executeGoStraightToEdge();
    void setGoStraightToEdge();

    void executeGetCloseToEdge();
    void setGetCloseToEdge();

    void executeFollowWall();
    void setFollowWall();

    void executeFollowUntilEdge();
    void setFollowUntilEdge();

    void executeOpenTurn();
    void setOpenTurn();

    void executeOpenEnding();
    void setOpenEnding();

    // Obstacle avoidance states
    // TODO: Refactor this states into classes

    // Keeps the recovery turn active until its timer expires.
    void exceuteRecoveryTurn();
    void setRecoveryTurn(float steeringTarget);

    void executeEvadeUntilEdge();
    void setEvadeUntilEdge();

    void executeApproachBlueLine();
    void setApproachBlueLine();


    void executeReverseAfterBlueLine();
    void setReverseAfterBlueLine();

    void executeForwardAfterReverse();
    void setForwardAfterReverse();

private:
    uint16_t SENSOR_DATA_TIMEOUT_MS = 127;
    uint16_t CAMERA_DATA_TIMEOUT_MS = 500;
    uint16_t INVALID_DISTANCE = MAX_VALID_DISTANCE + 1;
    uint8_t CAMERA_NOT_FOUND = 250;
    static constexpr int OBSTACLE_DRIVE_PWM = -80;
    static constexpr uint16_t BLUE_LINE_FRONT_TARGET_MM = 50;
    static constexpr uint32_t BLUE_LINE_REVERSE_TIME_MS = 3000;
    static constexpr float POST_REVERSE_STRAIGHT_DISTANCE_MM = 300.0f;


    // States shared by both runs; each firmware uses a subset.
    enum class TASK {
        UNDEFINED,
        GO_STRAIGHT_TO_EDGE,
        GET_CLOSE_TO_EDGE,
        FOLLOW_WALL,
        FOLLOW_UNTIL_EDGE,
        OPEN_TURN,
        OPEN_ENDING,
        FINISHED,
        EVADE_UNTIL_EDGE,
        APPROACH_BLUE_LINE,
        REVERSE_AFTER_BLUE_LINE,
        FORWARD_AFTER_REVERSE,
    };

    int wallDistance = 300;

    TASK taskStatus = TASK::UNDEFINED;
    TASK previousTaskStatus = TASK::UNDEFINED;
    int lapCount = 0;
    bool finish = false;

    elapsedMillis frontDataAge;
    elapsedMillis backDataAge;
    elapsedMillis leftDataAge;
    elapsedMillis rightDataAge;
    elapsedMillis displayRefreshAge;
    elapsedMillis recoverySteering;
    elapsedMillis blueLineStableAge;
    elapsedMillis blueLineReverseAge;
    bool recoveryTurn = false;
    float recoveryAngle = 0;
    bool blueLineArmed = true;
    bool blueLineLastSample = false;

    OpenMVData vision;

    void changeTask(TASK newTask);
    static const char* taskName(TASK task);
    // Converts invalid or expired ToF readings to the INVALID_DISTANCE sentinel.
    bool updateSide(TOF4Walls::Side side, uint16_t& rawDistance,
                    uint16_t& filteredDistance, elapsedMillis& dataAge);
    void invalidateStaleSensorData();
    void refreshDebugDisplay();
};


#endif
