// 物理ボタン(G39=OK, G38=キャンセル)の配線確認用 実機テスト
// (.steering/20260926-add-button-hardware-test/design.md)
//
// 実行: pio test -e m5stack-atoms3
// 画面とシリアルの指示に従ってボタンを操作する。テスト中はタッチパッドに触れないこと。
//
// 配線はactive-low(OFF時HIGH/ON時LOW、未押下は外付けプルアップでHIGH/内蔵プルは無効)。
// OK/キャンセルとも物理スイッチのエッジのみで判定する(旧「右タッチでもOK扱い」の暫定対応は撤去済み)。
// ボタンの検証はピンレベル(digitalRead)を主にして、タッチのドリフトに惑わされないようにしている。

#include <M5Unified.h>
#include <unity.h>
#include "InputManager.h"
#include "SettingsStore.h"

namespace {
    // InputManagerのピン定数はprivateのため、テスト側でも同じ値を持つ
    const int PIN_BUTTON_OK = 39;     // G39
    const int PIN_BUTTON_CANCEL = 38; // G38

    const unsigned long WAIT_TIMEOUT_MS = 10000;  // 人の操作を待つ制限時間
    const unsigned long POLL_INTERVAL_MS = 10;
    const unsigned long RELEASE_STABLE_MS = 100;  // 離したと判定するのに必要な解放継続時間
    const unsigned long AFTER_RELEASE_MS = 200;   // 離した後のチャタリング監視時間
    const unsigned long IDLE_PIN_CHECK_MS = 1000;
    const unsigned long IDLE_EDGE_CHECK_MS = 3000;

    InputManager inputManager;
    SettingsStore settings;

    // active-low: 押下=ピンがLOW(OFF時HIGH/ON時LOW、内蔵プルは無効)
    bool pinPressed(int pin) {
        return digitalRead(pin) == LOW;
    }

    void showInstruction(const char *action, const char *target) {
        M5.Lcd.fillScreen(TFT_BLACK);
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
        M5.Lcd.setTextSize(1);
        M5.Lcd.drawString(action, M5.Lcd.width() / 2, M5.Lcd.height() / 2 - 16);
        M5.Lcd.setTextSize(2);
        M5.Lcd.setTextColor(TFT_YELLOW, TFT_BLACK);
        M5.Lcd.drawString(target, M5.Lcd.width() / 2, M5.Lcd.height() / 2 + 4);
        M5.Lcd.setTextSize(1);
        M5.Lcd.setTextDatum(BC_DATUM);
        M5.Lcd.setTextColor(TFT_DARKGREY, TFT_BLACK);
        M5.Lcd.drawString("NO TOUCH PAD", M5.Lcd.width() / 2, M5.Lcd.height() - 2);

        Serial.printf(">>> %s %s\n", action, target);
    }

    void pollInput() {
        inputManager.update(settings.thresholdDelta());
        delay(POLL_INTERVAL_MS);
    }

    // 対象ボタンを押す→離す操作を待ち受け、エッジ検出・独立性・チャタリングを確認する。
    // 押下の検出条件は「InputManagerがエッジを報告し、かつ対象ピンが実際に押下レベル」。
    // これによりタッチ由来の偽エッジ(ピンはHIGHのまま)では先に進まない。
    void checkButton(const char *name, int targetPin, int otherPin,
                     bool (InputManager::*targetPressed)() const) {
        char msg[96];

        // --- 押下待ち ---
        showInstruction("PRESS", name);
        bool pressed = false;
        unsigned long start = millis();
        while (millis() - start < WAIT_TIMEOUT_MS) {
            pollInput();

            // 独立性: 対象を押す前から反対のボタンのピンが押下レベルなら短絡/取り違え
            snprintf(msg, sizeof(msg), "%s: other button pin already pressed (short/miswire?)", name);
            TEST_ASSERT_FALSE_MESSAGE(pinPressed(otherPin), msg);

            if ((inputManager.*targetPressed)() && pinPressed(targetPin)) {
                snprintf(msg, sizeof(msg), "%s: other button pin also pressed (short/miswire?)", name);
                TEST_ASSERT_FALSE_MESSAGE(pinPressed(otherPin), msg);
                pressed = true;
                break;
            }
        }
        snprintf(msg, sizeof(msg), "%s: timeout waiting for press (button/edge not detected)", name);
        TEST_ASSERT_TRUE_MESSAGE(pressed, msg);

        // --- 離すまで待つ(押している間に追加のエッジが出ないこと) ---
        showInstruction("RELEASE", name);
        int extraEdges = 0;
        bool released = false;
        unsigned long releasedSince = 0;
        start = millis();
        while (millis() - start < WAIT_TIMEOUT_MS) {
            pollInput();
            if ((inputManager.*targetPressed)() && pinPressed(targetPin)) extraEdges++;

            snprintf(msg, sizeof(msg), "%s: other button pin pressed while holding (short/miswire?)", name);
            TEST_ASSERT_FALSE_MESSAGE(pinPressed(otherPin), msg);

            if (pinPressed(targetPin)) {
                releasedSince = 0;
            } else if (releasedSince == 0) {
                releasedSince = millis();
            } else if (millis() - releasedSince >= RELEASE_STABLE_MS) {
                released = true;
                break;
            }
        }
        snprintf(msg, sizeof(msg), "%s: timeout waiting for release (pin stays pressed)", name);
        TEST_ASSERT_TRUE_MESSAGE(released, msg);

        // --- 離した直後のチャタリングで誤検出しないこと ---
        start = millis();
        while (millis() - start < AFTER_RELEASE_MS) {
            pollInput();
            if ((inputManager.*targetPressed)() && pinPressed(targetPin)) extraEdges++;
        }
        snprintf(msg, sizeof(msg), "%s: press detected %d extra time(s) (chattering?)", name, extraEdges);
        TEST_ASSERT_EQUAL_INT_MESSAGE(0, extraEdges, msg);
    }
}

void setUp() {}
void tearDown() {}

// No.1 未押下時に両ピンが解放レベル(HIGH)。押下レベル(LOW)なら短絡か外付けプルアップ不良
void test_idle_pins_released() {
    showInstruction("DON'T", "TOUCH");
    unsigned long start = millis();
    while (millis() - start < IDLE_PIN_CHECK_MS) {
        TEST_ASSERT_FALSE_MESSAGE(pinPressed(PIN_BUTTON_OK), "OK: pin at pressed level while idle (short to GND / external pull-up not working?)");
        TEST_ASSERT_FALSE_MESSAGE(pinPressed(PIN_BUTTON_CANCEL), "CANCEL: pin at pressed level while idle (short to GND / external pull-up not working?)");
        delay(POLL_INTERVAL_MS);
    }
}

// No.2 OKボタン
void test_ok_button() {
    checkButton("OK", PIN_BUTTON_OK, PIN_BUTTON_CANCEL, &InputManager::okPressed);
}

// No.3 キャンセルボタン
void test_cancel_button() {
    checkButton("CANCEL", PIN_BUTTON_CANCEL, PIN_BUTTON_OK, &InputManager::cancelPressed);
}

// No.4 未押下のまま一定時間、ピンが押下レベルにならない。キャンセルはタッチ由来の
// 経路が無いので、エッジが立たないことも確認する(OKは右タッチ暫定対応があるため除外)
void test_idle_no_false_edges() {
    showInstruction("DON'T", "TOUCH");
    unsigned long start = millis();
    while (millis() - start < IDLE_EDGE_CHECK_MS) {
        pollInput();
        TEST_ASSERT_FALSE_MESSAGE(inputManager.cancelPressed(), "CANCEL: edge detected while idle (noise?)");
        TEST_ASSERT_FALSE_MESSAGE(pinPressed(PIN_BUTTON_OK), "OK: pin at pressed level while idle (noise/short?)");
        TEST_ASSERT_FALSE_MESSAGE(pinPressed(PIN_BUTTON_CANCEL), "CANCEL: pin at pressed level while idle (noise/short?)");
    }
}

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    M5.Lcd.setRotation(2); // アプリ本体(GUI::init)と同じ向き
    Serial.begin(115200);
    delay(2000); // USB CDCの接続待ち

    // アプリ本体と同じく、起動時(未接触)にタッチ基準値とボタン初期状態を記憶する
    inputManager.begin();

    UNITY_BEGIN();
    RUN_TEST(test_idle_pins_released);
    RUN_TEST(test_ok_button);
    RUN_TEST(test_cancel_button);
    RUN_TEST(test_idle_no_false_edges);
    UNITY_END();

    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextDatum(MC_DATUM);
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(TFT_GREEN, TFT_BLACK);
    M5.Lcd.drawString("DONE", M5.Lcd.width() / 2, M5.Lcd.height() / 2);
}

void loop() {}
