#include <Wire.h>

#include <ZumoShieldN.h>

#define MAX_COMMAND 10

// コマンドバッファと状態変数
char command[MAX_COMMAND];  // 最大20個まで

bool operationMode = false;
void doOperation();
int commandLength = 0;  // 実際の長さ
int initialDegree = 0;  // 初期角度

void setup() {
  // TODO: 初期角度を取得する
  Serial.begin(9600);
  buzzer.playOn();
  led.on();
  Serial.println("=== Command Input Mode ===");
}

void loop() {
  if (!operationMode) {
    commandInputLoop();  // 入力受付
  } else {
    doOperation();           // 実行
  }
}

// ==========================
// コマンド入力モード
// ==========================
void commandInputLoop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();  // 改行や空白の除去

    for (int i = 0; i < input.length(); i++) {
      char ch = input.charAt(i);
      commandLength++;

      if (isValidInput(ch)) {
        // 入力されたコマンドの長さのチェック
        if (commandLength >= MAX_COMMAND) {
          Serial.println("Error: command too long");
          commandLength = 0;
          return;
        }

        if (ch == '.') {
          command[commandLength + 1] = ch;
          Serial.print("Command: ");
          for (int j = 0; j < commandLength; j++) {
            Serial.print(command[j]);
          }
          Serial.println();
          Serial.println("== press button, start ==");
          operationMode = true;
          led.on();
          button.waitForButton();  // 開始待機
          led.off();
          return;
        }
      } else {
        Serial.print("Error: invalid input '");
        Serial.print(ch);
        Serial.println("'");
        commandLength = 0;
        Serial.println("=== Command Input Mode ===");
        return;
      }
    }

    // 最後に現在のコマンド列を表示（ピリオドが来てない場合）
    Serial.print("Current Command: ");
    for (int j = 0; j < commandLength; j++) {
      Serial.print(command[j]);
    }
    Serial.println();
  }
}


// 有効なコマンドかどうかをチェック
bool isValidInput(char ch) {
  return (ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch == '9') || (ch == 'a' || ch == 'b' || ch == '*' || ch == '.');
}

// ==========================
// コース逸脱チェック
// ==========================
// bool validateCommandPath(char ch) {
//   int y = 0;
//   int x = 0;
//   int dir = 0; // 0: ↑, 1: →, 2: ↓, 3: ←

//   // 今あるコマンド + 入力 ch を仮想的に実行
//   for (int i = 0; i <= commandLength; i++) {
//     char cmd = (i < commandLength) ? command[i] : ch;

//     // まず進む（f / r / l 共通）(1番最初は0,0になるためスキップする)
//     if (i != 0) {
//       if (dir == 0) y++;
//       else if (dir == 1) x++;
//       else if (dir == 2) y--;
//       else if (dir == 3) x--;
//     }
//     // 向き変更（r/l のみ）
//     if (cmd == 'r') {
//       dir = (dir + 1) % 4;
//     } else if (cmd == 'l') {
//       dir = (dir + 3) % 4;
//     }
//   }

//   //  ログ出力
//   Serial.print(" → (x, y) = (");
//   Serial.print(x);
//   Serial.print(", ");
//   Serial.print(y);
//   Serial.print("), dir = ");
//   Serial.print(dir);
//   Serial.println();

//   // 一つ先の交差点があるかをcheckする(範囲外check)
//   if (dir == 0) y++;
//   else if (dir == 1) x++;
//   else if (dir == 2) y--;
//   else if (dir == 3) x--;
//   if (x < 0 || x >= WIDTH_NUM || y < 0 || y >= HEIGHT_NUM) {
//     return false;
//   }

//   return true;
// }