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
    void beginUI();

    bool updateCamera();
    bool updateSensors();
    bool followWallByCM();
    bool followWallUntilWall();

    void taskFollowWallByCm(float travelCM,float speed,float acceleration,float deacceleration, float initSpeed, float finalSpeed);

    void taskFollowWallUntilWall();

    void executeTask();

    void printData();

    void turn();

    void taskTurn();

    void taskGoStraighUntilEdge();

    void goStraighUntilEdge();
    
    void steerByStanley(float stanleyWallGain, float stanleyHeadingGain);

    void beginData();

    void begin();

    bool evadeUntilEdge();

    bool isSeeingObject();

    bool goStraightByIMUCM();

    void taskGoStraightByIMUCM(float travelCM,float speed,float acceleration,float deacceleration, float initSpeed, float finalSpeed);

    bool goToEdge();

    void taskGoToEdge();

    void selectTask();
    
    void selectTaskObstacles();

    // Envia una muestra binaria al C6 sin esperar respuesta ni bloquear.
    void enviarTelemetria();

    void decideDir();
    int frontDistance = 0;

    float initialSetPoint = 0.0f;
    // -1 es en sentido de las manecillas del reloijs, +1 es en el sentido opuesto a las manecillas
    DIRECTIONS direction = DIRECTIONS::COUNTERCLOCKWISE;

private:
    static constexpr int CS_FRONT = 7;
    static constexpr int CS_RIGHT = 6;
    static constexpr int CS_LEFT = 4;
    static constexpr int CS_BACK = 5;
    static constexpr uint32_t SENSOR_DATA_TIMEOUT_MS = 127;
    static constexpr uint32_t CAMERA_DATA_TIMEOUT_MS = 500;
    static constexpr uint16_t INVALID_DISTANCE = MAX_VALID_DISTANCE + 1;
    static constexpr uint16_t CAMERA_NOT_FOUND = 250;
    static constexpr float CAMERA_CENTER_X = 160.0f;
    static constexpr float CAMERA_HEIGHT = 240.0f;
    static constexpr float CAMERA_HALF_FOV_DEG = 30.0f;
    static constexpr int OBSTACLE_DRIVE_PWM = -80;
    static constexpr uint16_t BLUE_LINE_FRONT_TARGET_MM = 50;
    static constexpr uint32_t BLUE_LINE_REVERSE_TIME_MS = 3000;
    static constexpr float POST_REVERSE_STRAIGHT_DISTANCE_MM = 300.0f;
    static constexpr float BLUE_LINE_REARM_DISTANCE_MM = 1000.0f;

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
        OBSTACLES_TURN,
        APPROACH_BLUE_LINE,
        REVERSE_AFTER_BLUE_LINE,
        FORWARD_AFTER_REVERSE,
        
    };


    int evadedCounter = 0;
    int wallDistance = 300;

    int cmTarget = 0;
    TASK taskStatus = TASK::UNDEFINED;
    TASK previousTaskStatus = TASK::UNDEFINED;
    int lapCount = 0;
    bool finish = false;

    elapsedMillis frontDataAge;
    elapsedMillis backDataAge;
    elapsedMillis leftDataAge;
    elapsedMillis rightDataAge;
    elapsedMillis displayRefreshAge;
    elapsedMillis steeringCommandAge;
    elapsedMillis recoverySteering;
    elapsedMillis blueLineStableAge;
    elapsedMillis blueLineReverseAge;
    bool recoveryTurn = false;
    float recoveryAngle = 0;
    float lastSteeringCommand = 0.0f;
    bool steeringCommandInitialized = false;
    bool blueLineArmed = true;
    bool blueLineLastSample = false;

    OpenMVData vision;

    void changeTask(TASK newTask);
    static const char* taskName(TASK task);
    bool updateSide(TOF4Walls::Side side, uint16_t& rawDistance,
                    uint16_t& filteredDistance, elapsedMillis& dataAge);
    void invalidateStaleSensorData();
    void refreshDebugDisplay();
    float smoothSteeringCommand(float targetAngle);
};


#endif
