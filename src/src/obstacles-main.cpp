#include "Robot.h"
#include <SPI.h>

Robot robot;
elapsedMillis timerTask = 0;

int tareas = 0;

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
  timerTask = 0;
}

void loop() {
  robot.imu.update();
  robot.updateSensors();
  robot.updateCam();
  robot.printData();
  //robot.executeTaskObstacles();
  if(tareas == 0){
    if(timerTask < 1000){
        robot.ackermann.setSteeringAngle(25);
    }
    else{
      tareas = 1;
      timerTask = 0;
    }
  }
  else if(tareas == 1){
    if(timerTask < 500){
      robot.move.driveAtPWM(40);
    }
    else{
      tareas = 2;
      timerTask = 0; 
    } 
  }
  else if(tareas == 2){
    if(timerTask < 1000){
      robot.move.controller.brake();
      robot.ackermann.setSteeringAngle(-25);      
    }
    else{
      tareas = 3;
      timerTask = 0; 
    } 
  }
  else if(tareas == 3){
    if(timerTask < 400){
      robot.move.driveAtPWM(-40);
    }
    else{
      tareas = 4;
      timerTask = 0; 
    }
  }
  else if(tareas == 4){
    if(timerTask < 1000){
      robot.ackermann.setSteeringAngle(25);
      robot.move.controller.brake();
    }
    else{
      timerTask = 0;
      tareas++;
    }
  }
  else if( tareas == 5){
    if(timerTask < 400){
      robot.move.driveAtPWM(40);
    }
    else{
      timerTask = 0;
      tareas++;
    }
  }
  else if( tareas == 6){
    if(timerTask < 1000){
      robot.ackermann.setSteeringAngle(-25);
      robot.move.controller.brake();
    }
    else{
      timerTask = 0;
      tareas++;
    }
  }
  else if( tareas == 7){
    if(timerTask < 1000){
      robot.move.driveAtPWM(-40);
    }
    else{
      timerTask = 0;
      tareas++;
    }
  }
  else{
    robot.move.controller.brake();
  }
}
