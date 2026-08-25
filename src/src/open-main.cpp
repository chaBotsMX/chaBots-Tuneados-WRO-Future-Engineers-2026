#include "Robot.h"

Robot robot;

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
  robot.selectTask();
}

