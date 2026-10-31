#include <Arduino.h>
#include "GameStateMachine.h"
#include "CollisionChecker.h"

GameStateMachine::GameStateMachine(InputManager &input, GUI &gui)
    : input_(input), gui_(gui), state(GameState::TITLE),
      titleMenuIndex(0), tutorialStep(0), tutorialStepStartMs(0), tutorialMovesSeenMask(0),
      playStartMs(0), survivalTimeMs(0),
      prevUp(false), prevDown(false), prevLeft(false), prevRight(false),
      upEdge(false), downEdge(false), leftEdge(false), rightEdge(false) {}

void GameStateMachine::begin() {
    state = GameState::TITLE;
    titleMenuIndex = 0;
}

unsigned long GameStateMachine::tickDelayMs() const {
    // プレイ中は滑らかさ優先で短周期。メニュー系は従来100msだったが、
    // 押下から画面反映までの遅延を縮めるため50msに短縮する(入力取りこぼしは
    // main.cppの高速ポーリング＋エッジラッチで別途防止済み)。
    return state == GameState::PLAYING ? 30 : 50;
}

int GameStateMachine::currentThresholdDelta() const {
    return settings.thresholdDelta();
}

void GameStateMachine::update() {
    upEdge = input_.up() && !prevUp;
    downEdge = input_.down() && !prevDown;
    leftEdge = input_.left() && !prevLeft;
    rightEdge = input_.right() && !prevRight;
    prevUp = input_.up();
    prevDown = input_.down();
    prevLeft = input_.left();
    prevRight = input_.right();

    switch (state) {
        case GameState::TITLE: updateTitle(); break;
        case GameState::TUTORIAL: updateTutorial(); break;
        case GameState::SETTINGS: updateSettings(); break;
        case GameState::DIFFICULTY_SELECT: updateDifficultySelect(); break;
        case GameState::PLAYING: updatePlaying(); break;
        case GameState::GAME_OVER: updateGameOver(); break;
    }
}

void GameStateMachine::updateTitle() {
    if (downEdge) titleMenuIndex = (titleMenuIndex < 2) ? titleMenuIndex + 1 : 2;
    if (upEdge) titleMenuIndex = (titleMenuIndex > 0) ? titleMenuIndex - 1 : 0;

    if (input_.okPressed()) {
        if (titleMenuIndex == 0) {
            startTutorial();
        } else if (titleMenuIndex == 1) {
            state = GameState::DIFFICULTY_SELECT;
        } else {
            state = GameState::SETTINGS;
        }
        return;
    }

    gui_.drawTitle(titleMenuIndex);
}

void GameStateMachine::startTutorial() {
    state = GameState::TUTORIAL;
    tutorialStep = 0;
    tutorialStepStartMs = millis();
    tutorialMovesSeenMask = 0;
    player.reset(TUTORIAL_DEMO_PLAYER_SIZE);
}

void GameStateMachine::updateTutorial() {
    if (input_.cancelPressed() || input_.okPressed()) {
        state = GameState::TITLE;
        return;
    }

    if (tutorialStep == 0) {
        player.move(input_.up(), input_.down(), input_.left(), input_.right());

        if (input_.up()) tutorialMovesSeenMask |= 0x1;
        if (input_.down()) tutorialMovesSeenMask |= 0x2;
        if (input_.left()) tutorialMovesSeenMask |= 0x4;
        if (input_.right()) tutorialMovesSeenMask |= 0x8;

        bool allMovesSeen = (tutorialMovesSeenMask == 0xF);
        bool timedOut = (millis() - tutorialStepStartMs) > TUTORIAL_STEP0_TIMEOUT_MS;
        if (allMovesSeen || timedOut) {
            tutorialStep = 1;
            tutorialStepStartMs = millis();
            wallManager.reset(DifficultyConfig::paramsForLevel(1));
        }
    } else if (tutorialStep == 1) {
        player.move(input_.up(), input_.down(), input_.left(), input_.right());
        wallManager.update(millis() - tutorialStepStartMs);
        if (CollisionChecker::check(player, wallManager)) {
            tutorialStep = 2;
        }
    }
    // tutorialStep == 2: 衝突後の説明表示。自機・壁とも動かさずキャンセル待ちにする

    gui_.drawTutorial(tutorialStep, player, wallManager);
}

void GameStateMachine::updateSettings() {
    // G38/G39配線不良の暫定対策: 設定選択を左右ではなく上下に変更(基板修正版が届き次第、左右に戻す)
    if (upEdge) settings.previous();
    if (downEdge) settings.next();

    if (input_.okPressed() || input_.cancelPressed()) {
        state = GameState::TITLE;
        return;
    }

    gui_.drawSettings(settings.current());
}

void GameStateMachine::updateDifficultySelect() {
    // G38/G39配線不良の暫定対策: レベル選択を左右ではなく上下に変更(基板修正版が届き次第、左右に戻す)
    if (upEdge) difficulty.previous();
    if (downEdge) difficulty.next();

    if (input_.cancelPressed()) {
        state = GameState::TITLE;
        return;
    }
    if (input_.okPressed()) {
        startPlaying();
        return;
    }

    gui_.drawDifficultySelect(difficulty.currentLevel());
}

void GameStateMachine::startPlaying() {
    state = GameState::PLAYING;
    DifficultyParams params = difficulty.params();
    player.reset(params.playerSize);
    wallManager.reset(params);
    playStartMs = millis();
}

void GameStateMachine::updatePlaying() {
    if (input_.cancelPressed()) {
        state = GameState::TITLE;
        return;
    }

    unsigned long elapsed = millis() - playStartMs;
    player.move(input_.up(), input_.down(), input_.left(), input_.right());
    wallManager.update(elapsed);

    gui_.drawPlaying(player, wallManager, elapsed);

    if (CollisionChecker::check(player, wallManager)) {
        survivalTimeMs = elapsed;
        state = GameState::GAME_OVER;
    }
}

void GameStateMachine::updateGameOver() {
    gui_.drawGameOver(survivalTimeMs);

    if (input_.okPressed() || input_.cancelPressed()) {
        state = GameState::TITLE;
    }
}
