#pragma once

#include "GameState.h"
#include "InputManager.h"
#include "GUI.h"
#include "SettingsStore.h"
#include "DifficultyConfig.h"
#include "Player.h"
#include "WallManager.h"

// GameStateの保持と画面遷移、状態ごとのロジック更新・描画呼び出しを行う(docs/functional-design.md 5章)
class GameStateMachine {
public:
    GameStateMachine(InputManager &input, GUI &gui);

    void begin();
    void update();

    // プレイ中は短い周期、それ以外は長めの周期でloop()を回す(design.md 3.3節)
    unsigned long tickDelayMs() const;

    // 現在のしきい値(感度)設定。main.cppがInputManager::update()に渡すために使う
    int currentThresholdDelta() const;

private:
    static const unsigned long TUTORIAL_STEP0_TIMEOUT_MS = 4000;
    static const int TUTORIAL_DEMO_PLAYER_SIZE = 10;

    InputManager &input_;
    GUI &gui_;

    GameState state;

    int titleMenuIndex; // 0:チュートリアル 1:ゲーム開始 2:設定

    int tutorialStep; // 0:移動練習 1:壁デモ 2:衝突後の説明
    unsigned long tutorialStepStartMs;
    unsigned int tutorialMovesSeenMask;

    SettingsStore settings;
    DifficultyConfig difficulty;
    Player player;
    WallManager wallManager;

    unsigned long playStartMs;
    unsigned long survivalTimeMs;

    bool prevUp, prevDown, prevLeft, prevRight;
    bool upEdge, downEdge, leftEdge, rightEdge;

    void updateTitle();
    void updateTutorial();
    void updateSettings();
    void updateDifficultySelect();
    void updatePlaying();
    void updateGameOver();

    void startTutorial();
    void startPlaying();
};
