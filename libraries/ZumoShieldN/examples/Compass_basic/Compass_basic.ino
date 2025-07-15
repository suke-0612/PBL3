#include <Wire.h>
#include <ZumoShieldN.h>

void setup() {
  Serial.begin(9600);

  imu.begin();

  imu.configureForCompassHeading();

  button.waitForButton();
   
  Serial.println("starting calibration");
  imu.doCompassCalibration();
//  imu.setCompassCalibration(, , , );
  
  Serial.print("max.x   ");
  Serial.print(imu.m_max.x);
  Serial.println();
  Serial.print("max.y   ");
  Serial.print(imu.m_max.y);
  Serial.println();
  Serial.print("min.x   ");
  Serial.print(imu.m_min.x);
  Serial.println();
  Serial.print("min.y   ");
  Serial.print(imu.m_min.y);
  Serial.println();

  button.waitForButton();
}

void loop() {
  float heading;

  heading = imu.averageCompassHeading();
  Serial.print("Heading: ");
  Serial.println(heading);
}
