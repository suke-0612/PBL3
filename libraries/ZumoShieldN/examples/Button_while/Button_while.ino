#include <Wire.h>
#include <ZumoShieldN.h>

void setup() {
  
}

void loop() {
  while(!button.isPressed());
  
  led.on();
  delay(1000);
  led.off();  
  delay(1000);
  led.on();
  delay(1000);
  led.off();    
}
