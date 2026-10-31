#pragma once

// タッチ4ch(G5-8)と物理ボタン2個(G39=OK/G38=キャンセル)の読み取り・判定を担う。
// ゲームロジック側にはtouchRead()等のハードウェアAPIを一切見せない。
class InputManager {
public:
    InputManager();

    // 起動時に呼び、タッチセンサーの未接触状態の基準値を記憶する
    void begin();

    // 1ループ分の入力を読み取る。thresholdDeltaはSettingsStoreが持つしきい値
    void update(int thresholdDelta);

    bool up() const;
    bool down() const;
    bool left() const;
    bool right() const;

    // OK/キャンセルは「押下エッジがあったか」を返す。update()を高頻度で呼んでも
    // エッジはラッチされ、1フレーム内の短い押下を取りこぼさない。
    bool okPressed() const;
    bool cancelPressed() const;

    // フレーム開始時に呼び、前フレームで消費済みの押下ラッチをクリアする。
    // デバウンス状態(stableState等)はクリアしないため、入力は連続的に追跡される。
    void clearEdges();

private:
    static const int PIN_TOUCH_UP = 6;     // G6
    static const int PIN_TOUCH_LEFT = 8;   // G8
    static const int PIN_TOUCH_RIGHT = 5;  // G5
    static const int PIN_TOUCH_DOWN = 7;   // G7
    static const int PIN_BUTTON_OK = 39;     // G39
    static const int PIN_BUTTON_CANCEL = 38; // G38
    static const unsigned long DEBOUNCE_MS = 30;

    int touchBaseline[4];
    bool touchActive[4];

    bool okStableState;
    bool okLastRawState;
    bool okPrevStableState;
    bool okEdge;             // 直近update()で検出した押下エッジ
    bool okEdgeLatched;      // フレーム内で一度でもエッジが立ったか(clearEdgesまで保持)
    unsigned long okLastChangeMs;

    bool cancelStableState;
    bool cancelLastRawState;
    bool cancelPrevStableState;
    bool cancelEdge;
    bool cancelEdgeLatched;
    unsigned long cancelLastChangeMs;

    bool readDebounced(int pin, bool &stableState, bool &lastRawState, unsigned long &lastChangeMs);
};
