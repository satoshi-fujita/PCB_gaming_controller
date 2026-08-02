#include <M5Unified.h>
#include "GUI.h"

#define TOUCH_PIN1 G5
#define TOUCH_PIN2 G6
#define TOUCH_PIN3 G7
#define TOUCH_PIN4 G8

int touchValue[4] = {0,0,0,0};

GUI gui;    // GUIクラスのインスタンス

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);

    // タッチセンサーの初期値記憶
    touchValue[0] = touchRead(TOUCH_PIN1);
    touchValue[1] = touchRead(TOUCH_PIN2);
    touchValue[2] = touchRead(TOUCH_PIN3);
    touchValue[3] = touchRead(TOUCH_PIN4);

    // GUI初期化
    gui.init(touchValue[0], touchValue[1], touchValue[2], touchValue[3]);

    // シリアル通信の初期化
    USBSerial.begin(115200);
    while (!USBSerial) { delay(10); } // シリアル接続待ち
    USBSerial.println("Touch Sensor Test Start");
}

void loop() {
    // タッチセンサーの値を読み取る
    touchValue[0] = touchRead(TOUCH_PIN1);
    touchValue[1] = touchRead(TOUCH_PIN2);
    touchValue[2] = touchRead(TOUCH_PIN3);
    touchValue[3] = touchRead(TOUCH_PIN4);

    // タッチセンサーの値をシリアルモニターに出力
    USBSerial.printf("Touch Value: %d, %d, %d, %d\n", touchValue[0], touchValue[1], touchValue[2], touchValue[3]);

    // GUIを描画
    gui.drawGUI(touchValue[0], touchValue[1], touchValue[2], touchValue[3]);

    // 少し待機
    delay(100);
}