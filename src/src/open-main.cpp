#include "Robot.h"
#include <SPI.h>

Robot robot;

void setup() {
  // put your setup code here, to run once:
  robot.ui.begin();
  delay(300);
  robot.ui.beginDebugDisplay(); // Comenta esta linea para desactivar todo el debug OLED.
  delay(300);
  robot.beginComms();
  robot.ackermann.begin();
  robot.validData.front = MAX_VALID_DISTANCE;
  robot.validData.left = MAX_VALID_DISTANCE;
  robot.validData.right = MAX_VALID_DISTANCE;

 while (robot.ui.buttonRead() == false) {
    robot.imu.update();
    if (robot.ui.isDisplayReady()) {
      robot.updateCamOpen(false); // Show camera reception without choosing the race direction.
    }
    if(robot.updateSensors() == true){
    robot.ui.neoColor(0,255,0);
  }
  }
  robot.ui.neoColor(0,0,255);
  robot.ui.buzzSound(1);
  robot.imu.setSetPoint(0);
} 

void loop() {
  robot.imu.update();
  robot.updateSensors();
  robot.updateCamOpen();
  robot.printData();

  robot.executeTaskOpen();
}
