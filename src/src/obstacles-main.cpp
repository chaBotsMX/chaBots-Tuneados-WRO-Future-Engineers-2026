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

  robot.move.setSteeringController(robot.ackermann);
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
int a = 0;
void loop() {
  robot.imu.update();
  robot.updateSensors();

  robot.updateCam();
  robot.printData();

  if(!robot.move.executeMoving()){
    return;
  }
  //robot.executeTaskObstacles();
  if(a == 0){
    robot.ackermann.setSteeringAngle(0);
    delay(1000);
    a++;
  }
  else if(a == 1){
    robot.move.turnByCm(50,35,0);
    a++;
  }

}
