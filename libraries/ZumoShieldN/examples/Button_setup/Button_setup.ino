#include <Wire.h>
#include <ZumoShieldN.h>

int led_reset_state = 0;

void setup() {
  led.off();
  if (button.isPressed()) {
    led_reset_state = 1;
  } else {
    led_reset_state = 0;
  }
}

void loop() {
  if (led_reset_state == 0) {
    if(button.isPressed()) {
      led.on();
    } else {
      led.off();
    }
  } else if (led_reset_state == 1) {
    if(button.isPressed()) {
      led.off();
    } else {
      led.on();
    }
  }
}
