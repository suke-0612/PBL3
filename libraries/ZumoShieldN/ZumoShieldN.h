#pragma once

#include <LSM303.h>
#include <L3G.h>
#include <Pushbutton.h>
#include <QTRSensors.h>
#ifdef ARDUINO_ARCH_AVR
#include <ZumoBuzzer.h>
#endif /* ARDUINO_ARCH_AVR */
#include <ZumoIMU.h>
#include <ZumoMotors.h>
#include <ZumoReflectanceSensorArray.h>

extern ZumoMotors  motors;

#ifdef ARDUINO_ARCH_AVR
#define ZUMO_LED 13
#endif /* ARDUINO_ARCH_AVR */

#ifdef ARDUINO_WIO_TERMINAL
#define ZUMO_LED 9
#undef ZUMO_BUTTON
#define ZUMO_BUTTON 48
#endif /* ARDUINO_WIO_TERMINAL */

class ZumoLED
{
  public:
	ZumoLED(uint8_t pin){_pin = pin; pinMode(_pin, OUTPUT);};
	void on(){digitalWrite(_pin, HIGH);};
	void off(){digitalWrite(_pin, LOW);};
	void set(int i){
		digitalWrite(_pin, (i == 1)? HIGH : LOW);
	};
  private:
	uint8_t _pin;
};

#ifdef ARDUINO_ARCH_AVR
class ZumoBuzzerN : public ZumoBuzzer
{
  public:
	ZumoBuzzerN(){};
	void playOn(void) {play(">g32>>c32");};
	void playStart(void) {playNote(NOTE_G(4), 500, 15);}
	void playNum(int cnt) {
		for (int i = 0; i < cnt; i++){
			delay(1000);
			playNote(NOTE_G(3), 50, 12);
		}  
	};
};
#endif /* ARDUINO_ARCH_AVR */

#ifdef ARDUINO_WIO_TERMINAL
class ZumoBuzzerN 
{
  public:
	ZumoBuzzerN(){};
	void playOn(void) {};
	void playStart(void) {};
	void playNum(int cnt) {
	};
};
#endif /* ARDUINO_WIO_TERMINAL */

class ZumoReflectanceSensorArrayN : public ZumoReflectanceSensorArray {
  public:
	unsigned int values[6];
	ZumoReflectanceSensorArrayN() : ZumoReflectanceSensorArray(QTR_NO_EMITTER_PIN){
		
	};
	void update(void){
		read(values);
	}
	unsigned int value(int i){
		if((i <= 6) && (i > 0)) {
			return values[i-1];
		}
		return 0;
	}
};

// Converts x and y components of a vector to a heading in degrees.
// This calculation assumes that the Zumo is always level.
template <typename T> float heading(ZumoIMU::vector<T> v, ZumoIMU::vector<int16_t> m_max, ZumoIMU::vector<int16_t> m_min)
{
  float x_scaled =  2.0*(float)(v.x - m_min.x) / (m_max.x - m_min.x) - 1.0;
  float y_scaled =  2.0*(float)(v.y - m_min.y) / (m_max.y - m_min.y) - 1.0;

  float angle = atan2(y_scaled, x_scaled)*180 / M_PI;
  if (angle < 0)
    angle += 360;
  return angle;
}

class ZumoIMUN : public ZumoIMU
{
  public:
	ZumoIMU::vector<int16_t> m_max; // maximum magnetometer values, used for calibration
	ZumoIMU::vector<int16_t> m_min; // minimum magnetometer values, used for calibration

	ZumoIMUN() {};

	void begin(void) {
		// Initialize the Wire library and join the I2C bus as a master
		Wire.begin();

#ifdef ARDUINO_WIO_TERMINAL
		delay(100);
#endif /* ARDUINO_WIO_TERMINAL */

		// Initialize IMU
		init();

		// Enables accelerometer and magnetometer
		enableDefault();

		configureForCompassHeading();
	};

	/* turnAngle is a 32-bit unsigned integer representing the amount
	the robot has turned since the last time turnSensorReset was
	called. This is computed solely using the Z axis of the gyro, so
	it could be inaccurate if the robot is rotated about the X or Y
	axes.

	Our convention is that a value of 0x20000000 represents a 45
	degree counter-clockwise rotation.  This means that a uint32_t
	can represent any angle between 0 degrees and 360 degrees.  If
	you cast it to a signed 32-bit integer by writing
	(int32_t)turnAngle, that integer can represent any angle between
	-180 degrees and 180 degrees. */
	uint32_t turnAngle = 0;

	int32_t turnAngleDegree = 0;

	// turnRate is the current angular rate of the gyro, in units of
	// 0.07 degrees per second.
	int16_t turnRate;

	// This is the average reading obtained from the gyro's Z axis
	// during calibration.
	int16_t gyroOffset;

	// This variable helps us keep track of how much time has passed
	// between readings of the gyro.
	uint16_t gyroLastUpdate = 0;

	// This constant represents a turn of 45 degrees.
	const int32_t turnAngle45 = 0x20000000;

	// This constant represents a turn of 90 degrees.
	const int32_t turnAngle90 = turnAngle45 * 2;

	// This constant represents a turn of approximately 1 degree.
	const int32_t turnAngle1 = (turnAngle45 + 22) / 45;

	int32_t turnSensorAngleDegree() {return turnAngleDegree;}
	// This should be called to set the starting point for measuring
	// a turn.  After calling this, turnAngle will be 0.
	void turnSensorReset() {
		gyroLastUpdate = micros();
		turnAngle = 0;
		turnAngleDegree = 0;
	  }

	// Read the gyro and update the angle.  This should be called as
	// frequently as possible while using the gyro to do turns.
	void turnSensorUpdate() {
		// Read the measurements from the gyro.
		readGyro();
		turnRate = g.z - gyroOffset;
		
		// Figure out how much time has passed since the last update (dt)
		uint16_t m = micros();
		uint16_t dt = m - gyroLastUpdate;
		gyroLastUpdate = m;
		
		// Multiply dt by turnRate in order to get an estimation of how
		// much the robot has turned since the last update.
		// (angular change = angular velocity * time)
		int32_t d = (int32_t)turnRate * dt;
		  
		// The units of d are gyro digits times microseconds.  We need
		// to convert those to the units of turnAngle, where 2^29 units
		// represents 45 degrees.  The conversion from gyro digits to
		// degrees per second (dps) is determined by the sensitivity of
		// the gyro: 0.07 degrees per second per digit.
		//
		// (0.07 dps/digit) * (1/1000000 s/us) * (2^29/45 unit/degree)
		// = 14680064/17578125 unit/(digit*us)
		turnAngle += (int64_t)d * 14680064 / 17578125;
		turnAngleDegree = ((int32_t)turnAngle / turnAngle1);
	};

	void configureForTurnSensing(void) {
		switch (type)
		{
			//v1.2
			case ZumoIMUType::LSM303D_L3GD20H:
			  // 800 Hz output data rate,
			  // low-pass filter cutoff 100 Hz
			  writeReg(L3GD20H_ADDR, L3GD20H_REG_CTRL1, 0b11111111);

			  // 2000 dps full scale
			  writeReg(L3GD20H_ADDR, L3GD20H_REG_CTRL4, 0b00100000);

			  // High-pass filter disabled
			  writeReg(L3GD20H_ADDR, L3GD20H_REG_CTRL5, 0b00000000);
			  break;
			//v1.3
		  case ZumoIMUType::LSM6DS33_LIS3MDL:
			// 0x7C = 0b01111100
			// ODR = 0111 (833 Hz (high performance)); FS_G = 11 (+/- 2000 dps full scale)
			writeReg(LSM6DS33_ADDR, LSM6DS33_REG_CTRL2_G, 0x7C);
			  break;
			default:
			  break;
		}

		// Delay to give the user time to remove their finger.
		delay(500);

		// Calibrate the gyro.
		int32_t total = 0;
		for (uint16_t i = 0; i < 1024; i++)
		{
			// Wait for new data to be available, then read it.
			while(!gyroDataReady() & 0x08);
			readGyro();
			// Add the Z axis reading to the total.
			total += g.z;
		}
		gyroOffset = total / 1024;

		// Display the angle (in degrees from -180 to 180) until the
		// user presses A.
		turnSensorReset();   
	}

	void doCompassCalibration(void) {
		// The highest possible magnetic value to read in any direction is 32767
		// The lowest possible magnetic value to read in any direction is -32767
		ZumoIMU::vector<int16_t> running_min = {32767, 32767, 32767}, running_max = {-32767, -32767, -32767};
		unsigned char index;

		// To calibrate the magnetometer, the Zumo spins to find the max/min
		// magnetic vectors. This information is used to correct for offsets
		// in the magnetometer data.
		motors.setLeftSpeed(200);
		motors.setRightSpeed(-200);

		for(index = 0; index < 70; index ++)
		  {
			  // Take a reading of the magnetic vector and store it in compass.m
			  readMag();

			  running_min.x = min(running_min.x, m.x);
			  running_min.y = min(running_min.y, m.y);

			  running_max.x = max(running_max.x, m.x);
			  running_max.y = max(running_max.y, m.y);

			  Serial.println(index);
			  
			  delay(50);
		  }

		motors.setLeftSpeed(0);
		motors.setRightSpeed(0);

		Serial.print("max.x, max.y, min.x, min.y = ");
		Serial.print(running_max.x);
		Serial.print(", ");
		Serial.print(running_max.y);
		Serial.print(", ");
		Serial.print(running_min.x);
		Serial.print(", ");
		Serial.print(running_min.y);
		Serial.println();

		// Store calibrated values in m_max and m_min
		m_max.x = running_max.x;
		m_max.y = running_max.y;
		m_min.x = running_min.x;
		m_min.y = running_min.y;
	};

	void setCompassCalibration(int m_max_x, int m_max_y, int m_min_x, int m_min_y) {
		m_max.x = m_max_x;
		m_max.y = m_max_y;
		m_min.x = m_min_x;
		m_min.y = m_min_y;
	};

	// Average 10 vectors to get a better measurement and help smooth out
	// the motors' magnetic interference.
	float averageCompassHeading() {
		ZumoIMU::vector<int32_t> avg = {0, 0, 0};

		for(int i = 0; i < 10; i ++) {
			readMag();
			avg.x += m.x;
			avg.y += m.y;
		}
		avg.x /= 10.0;
		avg.y /= 10.0;

		// avg is the average measure of the magnetic vector.
		return heading(avg, m_max, m_min);
	};
};



ZumoLED     led(ZUMO_LED);
Pushbutton  button(ZUMO_BUTTON);
ZumoBuzzerN buzzer;
ZumoMotors  motors;
ZumoReflectanceSensorArrayN reflectances;
ZumoIMUN imu;
