#include <Wire.h>
#include <ZumoShieldN.h>

void setup() {
  Serial.begin(9600);  
  Serial.println("Zumo sample Start!");
  Serial.println(" 1 : Turn On LED.");
  Serial.println(" 2 : Turn OFF LED.");
}

void loop() {
  char led_control;
  if (Serial.available() > 0) {
    led_control = Serial.read();      
    if (led_control == '1') {
      led.on();
    }
    else if (led_control == '2') {
      led.off();
    }    
  }
}
