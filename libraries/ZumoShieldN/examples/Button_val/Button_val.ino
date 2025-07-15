#include <Wire.h>
#include <ZumoShieldN.h>

int led_state = 0;

void setup() {
  
}

void loop() {
  button.waitForPress();
  button.waitForRelease();
  if(led_state == 0) {
    led_state = 1;
    led.on();
  }else if(led_state == 1) {
    led_state = 0;
    led.off();    
  }
}
