#include <Wire.h>
#include <ZumoShieldN.h>
#include <math.h>

#define MOTOR_SPEED 120
#define TURN_SPEED 100
#define REFLECTANCE_THRESHOLD 600
#define LINE_TRACE_ADJUST_SPEED 50     //ライントレースの調整速度
#define ANGLE_TOLERANCE_DEGREE 1.25    //回転時に初期位置の向きから決めた四方向と1.25度以内になったら回転終了
#define ANGLE_TOLERANCE_MARGIN 358.75  //もしくは358.75度以上になったら回転終了

extern char route[];
extern float four_direction[];

void doOperation() {
  char cmd = route[0];

  if (cmd == 'r' || cmd == 'l' || cmd == 'u') {  //コマンドがrかlかuのときはdoTurn()を行う。
    doTurn(cmd);

  } else if (cmd == 'f') {
    goStraight();
  } else if (cmd == '.') {  //コマンドが.のときはブザーを鳴らし運行を終了する。
    motors.setSpeeds(0, 0);
    buzzer.playOn();
    while (true) {
      led.on();
      delay(1000);
      led.off();
      delay(1000);
    }
  }

  int i = 0;
  while (route[i] != NULL) {
    route[i] = route[i + 1];  //コマンドを詰める
    i++;
  }
}

void goStraight() {
  unsigned int sensorValues[6];

  while (true) {
    reflectances.read(sensorValues);

    //各センサーが黒を検出したらそれぞれ1になる
    bool s1 = (sensorValues[0] > REFLECTANCE_THRESHOLD);
    bool s6 = (sensorValues[5] > REFLECTANCE_THRESHOLD);
    bool s2 = (sensorValues[1] > REFLECTANCE_THRESHOLD);
    bool s5 = (sensorValues[4] > REFLECTANCE_THRESHOLD);



    if (s1 || s6) {  //一番外側のセンサーが黒色を検知したとき
      motors.setSpeeds(MOTOR_SPEED, MOTOR_SPEED);
      delay(300);  //交差点の真ん中まで進む
      motors.setSpeeds(0, 0);
      Serial.println("== Straight Stop==");
      break;
    }

    //ライントレース
    if (s2) {  //進行方向の右側に車体がずれているとき
      motors.setSpeeds(-MOTOR_SPEED + LINE_TRACE_ADJUST_SPEED, MOTOR_SPEED);
    } else if (s5) {  //進行方向の左側に車体がずれているとき
      motors.setSpeeds(MOTOR_SPEED, -MOTOR_SPEED + LINE_TRACE_ADJUST_SPEED);
    } else {  //ずれがないとき
      motors.setSpeeds(MOTOR_SPEED, MOTOR_SPEED);
    }
  }
}
//右に回ると数字増える　0以上360未満
void doTurn(char cmd) {
  if (cmd == 'r') {                             //コマンドがrのとき
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);  //右回転

    delay(300);

    while (four_direction_check(imu.averageCompassHeading()))
      ;

  } else if (cmd == 'l') {                      //コマンドがlのとき
    motors.setSpeeds(-TURN_SPEED, TURN_SPEED);  //左回転

    delay(300);

    while (four_direction_check(imu.averageCompassHeading()))
      ;
  } else if (cmd == 'u') {
    motors.setSpeeds(TURN_SPEED, -TURN_SPEED);  //右回転でUターン
    delay(300);

    while (four_direction_check(imu.averageCompassHeading()))
      ;

    delay(300);

    while (four_direction_check(imu.averageCompassHeading()))
      ;
  }

  motors.setSpeeds(0, 0);
  Serial.print("== Turn end ==");
}

bool four_direction_check(float current) {
  if (abs(four_direction[0] - current) < ANGLE_TOLERANCE_DEGREE || abs(four_direction[1] - current) < ANGLE_TOLERANCE_DEGREE || abs(four_direction[2] - current) < ANGLE_TOLERANCE_DEGREE || abs(four_direction[3] - current) < ANGLE_TOLERANCE_DEGREE || abs(four_direction[0] - current) > ANGLE_TOLERANCE_MARGIN || abs(four_direction[1] - current) > ANGLE_TOLERANCE_MARGIN || abs(four_direction[2] - current) > ANGLE_TOLERANCE_MARGIN || abs(four_direction[3] - current) > ANGLE_TOLERANCE_MARGIN) {
    return 0;
  }
  return 1;
}