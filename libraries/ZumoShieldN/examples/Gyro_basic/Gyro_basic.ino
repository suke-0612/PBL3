#include <Wire.h>
#include <ZumoShieldN.h>

void setup()
{  
  buzzer.playOn();  

  imu.begin();
  imu.configureForTurnSensing();
  imu.turnSensorReset();
  
  buzzer.playStart();
  Serial.begin(9600);
}

void loop()
{
  if(button.isPressed()) {
    imu.turnSensorReset();
  }
  imu.turnSensorUpdate();
  Serial.println(imu.turnSensorAngleDegree());
}

