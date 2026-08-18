#include "Robot.h"

Robot robot;

int taskNumber = 0;
int lap = 0;
int finish = 0;
void setup() {
  // put your setup code here, to run once:
  robot.beginComms();
  delay(1000);
  robot.ui.begin();
  robot.ackermann.begin();
  robot.validData.front = MAX_VALID_DISTANCE;
  robot.validData.left = MAX_VALID_DISTANCE;
  robot.validData.right = MAX_VALID_DISTANCE;

  while (robot.ui.buttonRead() == false) {
    if(robot.updateSensors() == true){
    robot.ui.neoColor(0,255,0);
  }
    if (robot.ui.buttonRead() == true) {
  
      robot.ui.neoColor(0,0,255);
      robot.ui.buzzSound(1);
    }
  }
  robot.imu.setSetPoint(0);
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
    robot.ui.buzzSound(1);
    robot.validData.front = MAX_VALID_DISTANCE;
    if (taskNumber == 1 && lap == 0){
      robot.taskGoStraightByIMUCM(FIRST_GO_STRAIGHT_DISTANCE,FIRST_GO_STRAIGHT_SPEED,30,30,0,0);
    }
    else if(taskNumber == 2 && lap == 0){
      robot.taskGoToEdge();
    }
    else if(taskNumber == 1 ){
      robot.taskFollowWallByCm(110,80,30,30,0,0);
    }
    else if(taskNumber == 2){
      robot.taskFollowWallUntilWall();
      robot.ui.buzzSound(3,1000,1000);
    }
    else if (taskNumber == 3){
      robot.validData.front = MAX_VALID_DISTANCE;
      robot.ui.buzzSound(2,1000,1000);
      robot.taskTurn();
    }
    else if(lap == 11){
      robot.move.driveAtPWM(-100);
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

