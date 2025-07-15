#include <Wire.h>
#include <ZumoShieldN.h>

void setup() {
  buzzer.playOn();
}

void loop() {
  button.waitForPress();

  // run left motor forward
  led.on();
  for (int speed = 0; speed <= 400; speed++) {
    motors.setLeftSpeed(speed);
    delay(2);
  }

  for (int speed = 400; speed >= 0; speed--) {
    motors.setLeftSpeed(speed);
    delay(2);
  }

  // run left motor backward
  led.off();
  for (int speed = 0; speed >= -400; speed--) {
    motors.setLeftSpeed(speed);
    delay(2);
  }

  for (int speed = -400; speed <= 0; speed++) {
    motors.setLeftSpeed(speed);
    delay(2);
  }

  // run right motor forward
  led.on();
  buzzer.playNum(1);
  for (int speed = 0; speed <= 400; speed++){
    motors.setRightSpeed(speed);
    delay(2);
  }

  for (int speed = 400; speed >= 0; speed--){
    motors.setRightSpeed(speed);
    delay(2);
  }

  // run right motor backward
  led.off();
  buzzer.playNum(1);

  for (int speed = 0; speed >= -400; speed--){
    motors.setRightSpeed(speed);
    delay(2);
  }

  for (int speed = -400; speed <= 0; speed++){
    motors.setRightSpeed(speed);
    delay(2);
  }

  delay(500);
}
