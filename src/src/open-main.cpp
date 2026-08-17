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
  robot.validData.front = 3000;
  robot.validData.left = 3000;
  robot.validData.right = 3000;

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

    if (taskNumber == 1 && lap == 0){
      robot.taskGoStraightByIMUCM(70,50,30,30,0,0);
    }
    else if(taskNumber == 2 && lap == 0){
      robot.taskGoToEdge();
    }
    else if(taskNumber == 1 ){
      robot.taskFollowWallByCm(110,80,30,30,0,0);
    }
    else if(taskNumber == 2){
      robot.taskFollowWallUntilWall();
    }
    else if (taskNumber == 3){
      robot.validData.front = 3000;
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

