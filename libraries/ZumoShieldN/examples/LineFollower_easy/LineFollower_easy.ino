#include <Wire.h>
#include <ZumoShieldN.h>

#define REFLECTANCE_THRESHOLD  600

void setup() {
  buzzer.playOn();

  Serial.begin(9600);
  Serial.println("Zumo sample Start!");

  led.on();
  button.waitForButton();
  led.off();
  buzzer.playStart();
  delay(1000);
}

void loop() {
  reflectances.update();
  
  if(reflectances.value(2) > REFLECTANCE_THRESHOLD) {
       motors.setSpeeds(-100, 100);
  }else if (reflectances.value(5) > REFLECTANCE_THRESHOLD) {
       motors.setSpeeds(100, -100);
  }else {
        motors.setSpeeds(100, 100);
  }  
}
