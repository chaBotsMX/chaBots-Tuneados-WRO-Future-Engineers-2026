#include "Robot.h"

Robot robot;

void setup() {
  // put your setup code here, to run once:
  robot.beginComms();
  robot.ui.begin();
  robot.ackermann.begin();
  robot.move.setTask(200, 50,10,10,0,0); // Example: Move 100 cm at 50 cm/s with acceleration and deceleration of 100 cm/s^2
  robot.validData.front = 3000;
  robot.validData.left = 3000;
  robot.validData.right = 3000;

  while (robot.ui.buttonRead() == false) {
    if(robot.updateSensors() == true){
    robot.ui.neoColor(0,255,0);
  }
    if (robot.ui.buttonRead() == true) {
  
      robot.ui.neoColor(0,0,255);
      robot.ui.buzzSound();
    }
  }
}

void loop() {
  int taskNumber = 0;
  robot.imu.update();
  robot.xiao.readData();
  robot.updateSensors();

  if(robot.taskStatus == 0){
    taskNumber++;

    if(taskNumber == 1){
      robot.taskFollowWallByCm(100,50,30,30,0,0);
    }
    else if (taskNumber == 2){
      robot.taskFollowWallUntilWall();
    }
    else{
      robot.move.driveAtPWM(0);
    }
  }
  else{
    robot.executeTask();
  }

}

