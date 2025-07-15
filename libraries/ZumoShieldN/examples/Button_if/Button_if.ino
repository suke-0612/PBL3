#include <Wire.h>
#include <ZumoShieldN.h>

void setup() {
  
}

void loop() {
  if(button.isPressed()) {
    led.on();
  }else{
    led.off();
  }
}
