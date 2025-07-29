#include <Wire.h>
#include <ZumoShieldN.h>
#include <math.h>

#define MAX_COMMAND 10

// コマンドバッファと状態変数
char destinationCommands[MAX_COMMAND] = {};

void doOperation();
void doRouteSearch();
int commandLength = 0;  // 入力済みコマンドの長さ
float four_direction[4];

void setup() {
  Serial.begin(9600);
  buzzer.playOn();
  led.on();
  Serial.println("=== Command Input Mode ===");
  bool isInputEnd = false;
  while (!isInputEnd) {
    isInputEnd = commandInputLoop();  // 入力受付
  }

  Serial.println();
  doRouteSearch();  // 経路探索
  Serial.println("== press button to calibrate ==");
  led.on();
  button.waitForButton();  // 開始待機
  imu.begin();
  imu.configureForCompassHeading();
  Serial.println("starting calibration");
  imu.doCompassCalibration();
  led.off();
  Serial.println("== press button to start ==");
  led.on();
  button.waitForButton();  // 開始待機
  four_direction[0] = imu.averageCompassHeading();
  four_direction[1] = fmod(imu.averageCompassHeading() + 90, 360);
  four_direction[2] = fmod(imu.averageCompassHeading() + 180, 360);
  four_direction[3] = fmod(imu.averageCompassHeading() + 270, 360);
}

void loop() {
  doOperation();  // 実行
}

// ==========================
// コマンド入力モード
// ==========================
bool commandInputLoop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();  // 改行や空白の除去
    Serial.println(input);

    if (commandLength + input.length() > MAX_COMMAND) {
      Serial.println("Error: command too long");
      return false;
    }

    for (int i = 0; i < input.length(); i++) {
      char ch = input.charAt(i);

      if (!isValidInput(ch)) {
        Serial.print("Error: invalid input '");
        Serial.print(ch);
        Serial.println("'");
        Serial.println("=== Command Input Mode ===");
        return false;
      }

      if ((commandLength + 1 == MAX_COMMAND) && !(ch == '.' || ch == '*')) {
        Serial.println("Error: last command must be '.' or '*'");
        return false;
      }

      if (ch == '.') {
        destinationCommands[commandLength] = ch;
        commandLength++;
        Serial.print("Command: ");
        for (int j = 0; j < commandLength; j++) {
          Serial.print(destinationCommands[j]);
        }
        return true;  // ループを終了させる
      } else if (ch == '*') {
        // ★修正点: commandLengthを0にするだけで良い
        commandLength = 0;
        Serial.println("command was deleted");
        Serial.println("=== Command Input Mode ===");  // モード表示を再度行うと親切
        return false;
      } else {
        destinationCommands[commandLength] = ch;
        commandLength++;
      }
    }
    // 最後に現在のコマンド列を表示（ピリオドが来てない場合）
    Serial.print("Current Command: ");
    for (int j = 0; j < commandLength; j++) {
      Serial.print(destinationCommands[j]);
    }
    Serial.println();
    return false;
  }

  // ★修正点: シリアル入力がない場合も必ずfalseを返す
  return false;
}

// 有効なコマンドかどうかをチェック
bool isValidInput(char ch) {
  return (ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch == '9' || ch == 'a' || ch == 'b' || ch == 'A' || ch == 'B' || ch == '*' || ch == '.');
}
