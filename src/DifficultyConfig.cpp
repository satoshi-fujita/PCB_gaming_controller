#include "DifficultyConfig.h"

namespace {
    const int WALL_THICKNESS = 10;

    const DifficultyParams LEVEL_1 = {10, 44, 26, 0.3f, 1350, 700, 5.0f, WALL_THICKNESS};
    const DifficultyParams LEVEL_2 = {14, 38, 22, 0.5f, 1150, 600, 8.0f, WALL_THICKNESS};
    const DifficultyParams LEVEL_3 = {18, 34, 18, 0.8f, 950, 500, 12.0f, WALL_THICKNESS};
}

DifficultyConfig::DifficultyConfig() : level(1) {}

int DifficultyConfig::currentLevel() const {
    return level;
}

void DifficultyConfig::next() {
    if (level < 3) level++;
}

void DifficultyConfig::previous() {
    if (level > 1) level--;
}

DifficultyParams DifficultyConfig::params() const {
    return paramsForLevel(level);
}

DifficultyParams DifficultyConfig::paramsForLevel(int level) {
    switch (level) {
        case 1: return LEVEL_1;
        case 3: return LEVEL_3;
        case 2:
        default: return LEVEL_2;
    }
}
