#pragma once

#include "Wall.h"
#include "DifficultyConfig.h"

// 壁の生成・スクロール・消去、経過時間に応じた開口/出現間隔の縮小(design.md 3.3, 3.4節)
class WallManager {
public:
    WallManager();

    void reset(const DifficultyParams &params);

    // elapsedMs: プレイ開始からの経過時間(ms)。1回の呼び出しで壁を2pxスクロールする
    void update(unsigned long elapsedMs);

    const Wall *walls() const;
    int wallCount() const;

private:
    static const int MAX_WALLS = 8;
    static const int SCREEN_WIDTH = 128;
    static const int SCREEN_HEIGHT = 128;
    static const int SCROLL_STEP = 2;
    static const int EDGE_MARGIN = 6; // 開口が画面端すぎに寄らないための余白

    Wall walls_[MAX_WALLS];
    DifficultyParams params_;
    unsigned long nextSpawnAtMs;

    int currentGapHeight(unsigned long elapsedMs) const;
    int currentSpawnIntervalMs(unsigned long elapsedMs) const;
};
