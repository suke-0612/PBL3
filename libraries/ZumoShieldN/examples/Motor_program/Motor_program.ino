#include <Wire.h>
#include <ZumoShieldN.h>

void setup() {
  buzzer.playOn();
}

void loop() {
  button.waitForPress();

  led.on();
  motors.setSpeeds(100, 100);
  delay(1000);
  motors.setSpeeds(0, 0);
  
  led.off();
  motors.setSpeeds(-100, -100);
  delay(1000);
  motors.setSpeeds(0, 0);

  led.on();
  buzzer.playNum(1);
  motors.setSpeeds(-100, 100);
  delay(1000);
  motors.setSpeeds(0, 0);

  led.off();
  buzzer.playNum(1);
  motors.setSpeeds(100, -100);
  delay(1000);
  motors.setSpeeds(0, 0);
  
}
