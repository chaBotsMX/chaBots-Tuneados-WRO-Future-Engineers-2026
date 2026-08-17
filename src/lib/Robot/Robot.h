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

    void decideDir();
    int taskStatus = 0;
    int frontDistance = 0;

    int initialSetPoint = 0;
    // -1 es en sentido de las manecillas del reloijs, +1 es en el sentido opuesto a las manecillas
    int direction = 0;
private:

    int evadedCounter = 0;
    int wallDistance = 300;

    int cmTarget = 0;



};


#endif
