#pragma once

// タッチしきい値（感度）3段階の保持。電源OFF後は保持しない（毎回MIDから開始）。
enum class Sensitivity {
    SENS_LOW,
    SENS_MID,
    SENS_HIGH
};

class SettingsStore {
public:
    SettingsStore();

    Sensitivity current() const;
    void next();      // 十字キー(下/右)で次の段階へ
    void previous();  // 十字キー(上/左)で前の段階へ

    // 現在のSensitivityに対応する、キャリブレーション値からのdelta閾値
    int thresholdDelta() const;

private:
    Sensitivity level;
};
