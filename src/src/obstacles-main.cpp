#include "Robot.h"

Robot robot;

int taskNumber = 0;
int lap = 0;
int finish = 0;


void setup() {
  // put your setup code here, to run once:
  robot.begin();
}

void loop() {
  robot.printData();
  robot.imu.update();
  robot.updateSensors();
 if(finish == 1){
  return;
 }
  if(robot.taskStatus == 0){
    taskNumber++;

    if (taskNumber == 1 && lap == 0){
      robot.taskGoStraighUntilEdge();
    }
    else if(taskNumber == 1){
      robot.taskFollowWallByCm(130,80,30,30,0,0);
    }
    else if (taskNumber == 2){
      robot.taskFollowWallUntilWall();
    }
    else if (taskNumber == 3){
      robot.validData.front = 3000;
      robot.taskTurn();
    }
    else if(lap == 11){
      robot.taskFollowWallByCm(30,80,30,30,0,0);
      delay(1000);
      robot.move.driveAtPWM(0);
      finish = 1;
    }
    else{
      lap++;
      taskNumber = 0;
    }
  }
  else{
    robot.executeTask();
  }

}