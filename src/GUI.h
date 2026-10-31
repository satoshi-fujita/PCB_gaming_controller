/***********************************************************************/
/*                                                                     */
/*  FILE        :GUI.h                                                 */
/*  DESCRIPTION :GameStateごとの画面描画を担うRenderer                    */
/*                                                                     */
/***********************************************************************/

#pragma once

#include <M5Unified.h>
#include "Player.h"
#include "WallManager.h"
#include "SettingsStore.h"

class GUI {
public:
    GUI();

    void init();

    void drawTitle(int selectedIndex); // 0:チュートリアル 1:ゲーム開始 2:設定
    void drawTutorial(int step, const Player &player, const WallManager &wallManager);
    void drawSettings(Sensitivity current);
    void drawDifficultySelect(int level);
    void drawPlaying(const Player &player, const WallManager &wallManager, unsigned long survivalTimeMs);
    void drawGameOver(unsigned long survivalTimeMs);

private:
    int screenWidth, screenHeight;

    void drawPlayer(const Player &player);
    void drawWalls(const WallManager &wallManager);
    void drawControlHint();
};
