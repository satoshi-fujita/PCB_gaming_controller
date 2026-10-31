#include <M5Unified.h>
#include <driver/gpio.h>
#include "InputManager.h"

InputManager::InputManager() {
    for (int i = 0; i < 4; i++) {
        touchBaseline[i] = 0;
        touchActive[i] = false;
    }
    okStableState = false;
    okLastRawState = false;
    okPrevStableState = false;
    okEdge = false;
    okEdgeLatched = false;
    okLastChangeMs = 0;

    cancelStableState = false;
    cancelLastRawState = false;
    cancelPrevStableState = false;
    cancelEdge = false;
    cancelEdgeLatched = false;
    cancelLastChangeMs = 0;
}

void InputManager::begin() {
    touchBaseline[0] = touchRead(PIN_TOUCH_UP);
    touchBaseline[1] = touchRead(PIN_TOUCH_LEFT);
    touchBaseline[2] = touchRead(PIN_TOUCH_RIGHT);
    touchBaseline[3] = touchRead(PIN_TOUCH_DOWN);

    // ボタン配線はactive-low(押下でGNDに落ちる/非押下時はHIGH)。
    // 外付けは470kΩプルダウンのため非押下時HIGHを保持できず、HIGHの保持源は機種で異なる:
    //  - AtomS3 : G38/G39が内部I2C(SDA/SCL)でモジュール側プルアップがあり、
    //             470kΩプルダウンを上回るため内蔵プル無しでもHIGHになり動作していた
    //  - AtomS3R: 内部I2Cが別ピン(G0/G45)へ移りG38/G39にモジュール側プルアップが無いため、
    //             470kΩプルダウンでLOWに張り付き「常時押下」扱いになっていた
    // 内蔵プルアップ(約45kΩ)なら470kΩプルダウンとの分圧で非押下時≈3.0V(HIGH)となり、
    // 両機種で確実にHIGHを保持できるため、機種分岐せず常時有効化する。
    pinMode(PIN_BUTTON_OK, INPUT_PULLUP);
    pinMode(PIN_BUTTON_CANCEL, INPUT_PULLUP);
    // pinMode()だけでは内蔵プルの設定が残らない場合があるため、
    // ESP-IDFのGPIO APIで内蔵プルアップを明示的に有効化する
    gpio_set_pull_mode(static_cast<gpio_num_t>(PIN_BUTTON_OK), GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(static_cast<gpio_num_t>(PIN_BUTTON_CANCEL), GPIO_PULLUP_ONLY);

    // 起動直後の実際のピン状態で初期化し、電源投入時の誤エッジ検出を防ぐ(active-low: LOWで押下)
    unsigned long now = millis();
    okStableState = okLastRawState = okPrevStableState = (digitalRead(PIN_BUTTON_OK) == LOW);
    okLastChangeMs = now;
    okEdgeLatched = false;
    cancelStableState = cancelLastRawState = cancelPrevStableState = (digitalRead(PIN_BUTTON_CANCEL) == LOW);
    cancelLastChangeMs = now;
    cancelEdgeLatched = false;
}

bool InputManager::readDebounced(int pin, bool &stableState, bool &lastRawState, unsigned long &lastChangeMs) {
    bool raw = (digitalRead(pin) == LOW); // active-low配線前提: OFF時HIGH/押下時LOW
    unsigned long now = millis();

    if (raw != lastRawState) {
        lastChangeMs = now;
        lastRawState = raw;
    }
    if (now - lastChangeMs >= DEBOUNCE_MS) {
        stableState = raw;
    }
    return stableState;
}

void InputManager::update(int thresholdDelta) {
    const int pins[4] = {PIN_TOUCH_UP, PIN_TOUCH_LEFT, PIN_TOUCH_RIGHT, PIN_TOUCH_DOWN};
    for (int i = 0; i < 4; i++) {
        int value = touchRead(pins[i]);
        touchActive[i] = (value - touchBaseline[i]) >= thresholdDelta;
    }

    bool okNow = readDebounced(PIN_BUTTON_OK, okStableState, okLastRawState, okLastChangeMs);
    okEdge = okNow && !okPrevStableState;
    okPrevStableState = okNow;
    if (okEdge) okEdgeLatched = true; // フレーム内で検出したエッジをラッチし、取りこぼしを防ぐ

    bool cancelNow = readDebounced(PIN_BUTTON_CANCEL, cancelStableState, cancelLastRawState, cancelLastChangeMs);
    cancelEdge = cancelNow && !cancelPrevStableState;
    cancelPrevStableState = cancelNow;
    if (cancelEdge) cancelEdgeLatched = true;
}

void InputManager::clearEdges() {
    okEdgeLatched = false;
    cancelEdgeLatched = false;
}

bool InputManager::up() const { return touchActive[0]; }
bool InputManager::left() const { return touchActive[1]; }
bool InputManager::right() const { return touchActive[2]; }
bool InputManager::down() const { return touchActive[3]; }
bool InputManager::okPressed() const { return okEdgeLatched; }
bool InputManager::cancelPressed() const { return cancelEdgeLatched; }
