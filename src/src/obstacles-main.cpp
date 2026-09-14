#include "Robot.h"
#include <SPI.h>

Robot robot;

void setup() {
  // put your setup code here, to run once:
  delay(1000);
  robot.ui.begin();
  robot.ui.beginDebugDisplay(); // Comenta esta linea para desactivar todo el debug OLED.
  robot.beginComms();
  robot.ackermann.begin();
  robot.validData.front = MAX_VALID_DISTANCE;
  robot.validData.left = MAX_VALID_DISTANCE;
  robot.validData.right = MAX_VALID_DISTANCE;

 while (robot.ui.buttonRead() == false) {
    robot.imu.update();
    if (robot.ui.isDisplayReady()) {
      robot.updateCam();
    }
    if(robot.updateSensors() == true){
    robot.ui.neoColor(0,255,0);
  }
  }
  robot.ui.neoColor(0,0,255);
  robot.ui.buzzSound(1);
  robot.initialSetPoint = robot.imu.getYaw();
  robot.imu.setSetPoint(robot.initialSetPoint);
}

void loop() {
  robot.imu.update();
  robot.updateSensors();
  robot.updateCam();
  robot.printData();

  robot.executeTaskObstacles();
}
