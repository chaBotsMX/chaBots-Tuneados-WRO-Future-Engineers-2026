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
#include "Xiao_UART.h"
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


#define IMU_BAUD_RATE 115200
#define XIAO_BAUD_RATE 2000000
#define PC_SERIAL_BAUD_RATE 115200

#define IMU_SERIAL Serial4



class Robot {
public:
    Move move;
    SensorData data;
    SensorData validData;
    HCI ui;
    AckermannController ackermann;
    Xiao_UART xiao;
    IMU imu;
    TrajectoryController tc;
    TOF4Walls tofs;

    Robot();
    void beginComms();
    void beginUI();

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

    void decideDir();
    int frontDistance = 0;

    int initialSetPoint = 0;
    // -1 es en sentido de las manecillas del reloijs, +1 es en el sentido opuesto a las manecillas
    DIRECTIONS direction = DIRECTIONS::COUNTERCLOCKWISE;

private:
    static constexpr int CS_FRONT = 7;
    static constexpr int CS_RIGHT = 6;
    static constexpr int CS_LEFT = 4;
    static constexpr int CS_BACK = 5;
    static constexpr uint32_t SENSOR_DATA_TIMEOUT_MS = 127;
    static constexpr uint16_t INVALID_DISTANCE = MAX_VALID_DISTANCE + 1;

    enum class TASK {
        UNDEFINED,
        GO_STRAIGHT_TO_EDGE,
        GET_CLOSE_TO_EDGE,
        FOLLOW_WALL,
        FOLLOW_UNTIL_EDGE,
        OPEN_TURN,
        OPEN_ENDING,
        FINISHED
        
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

    void changeTask(TASK newTask);
    static const char* taskName(TASK task);
    bool updateSide(TOF4Walls::Side side, uint16_t& rawDistance,
                    uint16_t& filteredDistance, elapsedMillis& dataAge);
    void invalidateStaleSensorData();
    void refreshDebugDisplay();
};


#endif
