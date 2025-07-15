#include <Wire.h>
#include <ZumoShieldN.h>

void setup() {
   buzzer.playOn();
}

void loop() {
  button.waitForPress();
  buzzer.playNum(2);
  button.waitForRelease();
}
