#include <Wire.h>
#include <ZumoShieldN.h>


void setup() {
  buzzer.playOn();
  Serial.begin(9600);
  Serial.println("Zumo sample Start!");
}

void loop() {
  reflectances.update();
  
  Serial.print(reflectances.value(1));
  Serial.print(',');
  Serial.print(reflectances.value(2));
  Serial.print(',');  
  Serial.print(reflectances.value(3));
  Serial.print(',');
  Serial.print(reflectances.value(4));
  Serial.print(',');  
  Serial.print(reflectances.value(5));
  Serial.print(',');  
  Serial.print(reflectances.value(6));
  Serial.print(',');    
  Serial.println();
  
  delay(1000);
}
