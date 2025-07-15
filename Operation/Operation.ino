#include <Wire.h>
#include <ZumoShieldN.h>

#define MOTOR_SPEED 120
#define TURN_SPEED 120
#define REFLECTANCE_THRESHOLD 600

extern char command[];
extern float four_direction[];

void doOperation() {
    char cmd = command[0];

    if (cmd == 'r' || cmd == 'l' || cmd == 'u') { //コマンドがrかlかuのときはdoTurn()を行う。
        doTurn(cmd);     
           
    } else if (cmd == 'f') {
        goStraight();
    } else if (cmd == '.') {　//コマンドが.のときはブザーを鳴らし運行を終了する。
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
    while(command[i] != NULL){
        command[i] = command[i + 1];  //コマンドを詰める
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
            motors.setSpeeds(- MOTOR_SPEED + 50, MOTOR_SPEED);
        } else if (s5) {  //進行方向の左側に車体がずれているとき
            motors.setSpeeds(MOTOR_SPEED, - MOTOR_SPEED + 50);
        } else {  //ずれがないとき
            motors.setSpeeds(MOTOR_SPEED, MOTOR_SPEED);
        }
    }
}
//右に回ると数字増える　0以上360未満
void doTurn(char cmd) {
        float heading; //現在の向きを表す変数
        if (cmd == 'r') {  //コマンドがrのとき
        motors.setSpeeds(TURN_SPEED, -TURN_SPEED);  //右回転
        delay(100);
        
        while(four_dirction_check(imu.averageCompassHeading())) 
                
    } else if (cmd == 'l') {　　//コマンドがlのとき
        motors.setSpeeds(-TURN_SPEED, TURN_SPEED);  //左回転

        int detectCount = 0;
        delay(100);
        
        while(four_dirction_check(imu.averageCompassHeading())) 
    } else if (cmd == 'u') {
        motors.setSpeeds(TURN_SPEED, -TURN_SPEED);  //右回転
        delay(100);
        
        while(four_dirction_check(imu.averageCompassHeading())) 

        delay(100);
        
        while(four_dirction_check(imu.averageCompassHeading())) 

    }

    motors.setSpeeds(0, 0);
    Serial.print("== Turn end ==");
}

bool four_dirction_check(float current){
    if(abs(four_direction[0] - current) < 2.0 || abs(four_direction[1] - current) < 2.0 ||
    abs(four_direction[2] - current) < 2.0 || abs(four_direction[3] - current) < 2.0 || 
    abs(four_direction[0] - current) > 358 || abs(four_direction[1] - current) > 358 ||
    abs(four_direction[2] - current) > 358 || abs(four_direction[3] - current) > 358 ){
        return 0;
    }
    return 1; 
}