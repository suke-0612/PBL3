#include <Wire.h>
#include <ZumoShieldN.h>

#define REF_THRESHOLD  400

void setup() {
  buzzer.playOn();
  Serial.begin(9600);
  Serial.println("Zumo sample Start!");
  button.waitForPress();
}

void loop() {
  reflectances.update();

  while ((reflectances.value(1) < REF_THRESHOLD) && 
          (reflectances.value(6) < REF_THRESHOLD)) {
    motors.setSpeeds(100, 100);
    led.on();
    reflectances.update();
  }
  motors.setSpeeds(0, 0);
  led.off(); 
}
