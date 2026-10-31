#include <Arduino.h>
#include "WallManager.h"

WallManager::WallManager() : nextSpawnAtMs(0) {
    for (int i = 0; i < MAX_WALLS; i++) {
        walls_[i].active = false;
    }
}

void WallManager::reset(const DifficultyParams &params) {
    params_ = params;
    nextSpawnAtMs = 0;
    for (int i = 0; i < MAX_WALLS; i++) {
        walls_[i].active = false;
    }
}

int WallManager::currentGapHeight(unsigned long elapsedMs) const {
    float shrink = params_.gapShrinkPerSec * (elapsedMs / 1000.0f);
    int gap = params_.gapInitial - static_cast<int>(shrink);
    return gap < params_.gapMin ? params_.gapMin : gap;
}

int WallManager::currentSpawnIntervalMs(unsigned long elapsedMs) const {
    float shrink = params_.intervalShrinkPerSec * (elapsedMs / 1000.0f);
    int interval = params_.intervalInitialMs - static_cast<int>(shrink);
    return interval < params_.intervalMinMs ? params_.intervalMinMs : interval;
}

void WallManager::update(unsigned long elapsedMs) {
    int gapHeight = currentGapHeight(elapsedMs);

    // 既存の壁をスクロールし、画面外に出たものを消す
    for (int i = 0; i < MAX_WALLS; i++) {
        if (!walls_[i].active) continue;
        walls_[i].x -= SCROLL_STEP;
        if (walls_[i].x + walls_[i].thickness < 0) {
            walls_[i].active = false;
        }
    }

    // 出現タイミングが来ていたら新しい壁を生成する
    if (elapsedMs >= nextSpawnAtMs) {
        for (int i = 0; i < MAX_WALLS; i++) {
            if (!walls_[i].active) {
                walls_[i].active = true;
                walls_[i].x = SCREEN_WIDTH;
                walls_[i].thickness = params_.wallThickness;
                walls_[i].gapHeight = gapHeight;
                walls_[i].gapY = random(EDGE_MARGIN, SCREEN_HEIGHT - EDGE_MARGIN - gapHeight);
                break;
            }
        }
        nextSpawnAtMs = elapsedMs + currentSpawnIntervalMs(elapsedMs);
    }
}

const Wall *WallManager::walls() const {
    return walls_;
}

int WallManager::wallCount() const {
    return MAX_WALLS;
}
