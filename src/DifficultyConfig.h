#pragma once

// 難易度レベル1〜3ごとの自機サイズ・壁パラメータ(design.md 3.4節)
struct DifficultyParams {
    int playerSize;           // 自機バウンディングボックスの一辺(px)
    int gapInitial;           // 壁の開口 初期値(px)
    int gapMin;                // 壁の開口 下限値(px)
    float gapShrinkPerSec;     // 開口の縮小レート(px/秒)
    int intervalInitialMs;     // 壁の出現間隔 初期値(ms)
    int intervalMinMs;         // 壁の出現間隔 下限値(ms)
    float intervalShrinkPerSec; // 出現間隔の縮小レート(ms/秒)
    int wallThickness;         // 壁1本あたりの横幅(px)
};

class DifficultyConfig {
public:
    DifficultyConfig();

    int currentLevel() const; // 1〜3
    void next();
    void previous();

    DifficultyParams params() const;
    static DifficultyParams paramsForLevel(int level);

private:
    int level;
};
