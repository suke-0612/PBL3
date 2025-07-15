#include <Wire.h>
#include <ZumoShieldN.h>

int push_count;

void setup() {
  Serial.begin(9600);  
  Serial.println("Zumo sample Start!");
  push_count = 0;
}

void loop() {
  button.waitForPress();
  button.waitForRelease();
  push_count = push_count + 1;
  Serial.print("Push count : ");
  Serial.println(push_count);
}
