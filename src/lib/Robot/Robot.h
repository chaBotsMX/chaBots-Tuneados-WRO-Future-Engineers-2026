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

    int taskStatus = 0;
    int frontDistance = 0;

private:


    int wallDistance = 200;

};


#endif