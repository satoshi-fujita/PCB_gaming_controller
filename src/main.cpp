// pio test実行時はテスト側のsetup()/loop()を使うため、アプリ本体のエントリポイントを除外する
#ifndef PIO_UNIT_TESTING

#include <M5Unified.h>
#include "GUI.h"
#include "InputManager.h"
#include "GameStateMachine.h"

InputManager inputManager;
GUI gui;
GameStateMachine game(inputManager, gui);

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);

    inputManager.begin();
    gui.init();
    game.begin();
}

void loop() {
    // ボタン/タッチの実サンプリング周期。描画周期(tickDelayMs)とは独立に高速で回し、
    // 100ms未満の短い押下も取りこぼさずに反応を良くする。
    const unsigned long BUTTON_POLL_MS = 8;

    unsigned long frameMs = game.tickDelayMs();
    int thresholdDelta = game.currentThresholdDelta();

    // このフレーム分の押下を集める前に、前フレームで消費済みのラッチをクリアする
    inputManager.clearEdges();

    // 1フレームの時間を細かく分割してポーリング。押下エッジはInputManager側で
    // ラッチされ、フレーム末のgame.update()でまとめて消費される。
    unsigned long frameStart = millis();
    do {
        inputManager.update(thresholdDelta);
        delay(BUTTON_POLL_MS);
    } while (millis() - frameStart < frameMs);

    game.update();
}

#endif // PIO_UNIT_TESTING
