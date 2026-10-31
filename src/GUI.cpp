/***********************************************************************/
/*                                                                     */
/*  FILE        :GUI.cpp                                               */
/*  DESCRIPTION :GameStateごとの画面描画を担うRenderer                    */
/*                                                                     */
/***********************************************************************/

#include <cstdio>
#include "GUI.h"

namespace {
    const char *MENU_LABELS[3] = {"TUTORIAL", "START", "SETTINGS"};
}

GUI::GUI() : screenWidth(0), screenHeight(0) {}

void GUI::init() {
    M5.Lcd.setRotation(2); // 画面表示を180°回転
    screenWidth = M5.Lcd.width();
    screenHeight = M5.Lcd.height();
}

void GUI::drawPlayer(const Player &player) {
    M5.Lcd.fillCircle(player.x(), player.y(), player.size() / 2, TFT_CYAN);
}

void GUI::drawWalls(const WallManager &wallManager) {
    const Wall *walls = wallManager.walls();
    for (int i = 0; i < wallManager.wallCount(); i++) {
        if (!walls[i].active) continue;
        int gapBottom = walls[i].gapY + walls[i].gapHeight;
        M5.Lcd.fillRect(walls[i].x, 0, walls[i].thickness, walls[i].gapY, TFT_RED);
        M5.Lcd.fillRect(walls[i].x, gapBottom, walls[i].thickness, screenHeight - gapBottom, TFT_RED);
    }
}

void GUI::drawControlHint() {
    M5.Lcd.setTextDatum(BC_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_DARKGREY, TFT_BLACK);
    M5.Lcd.drawString("[+]MOVE [OK]SELECT", screenWidth / 2, screenHeight - 10);
    M5.Lcd.drawString("[C]BACK", screenWidth / 2, screenHeight - 1);
}

void GUI::drawTitle(int selectedIndex) {
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.drawString("IRAIRA BOU", screenWidth / 2, 6);

    for (int i = 0; i < 3; i++) {
        M5.Lcd.setTextDatum(TL_DATUM);
        int y = 30 + i * 14;
        M5.Lcd.setTextColor(i == selectedIndex ? TFT_YELLOW : TFT_WHITE, TFT_BLACK);
        M5.Lcd.setCursor(20, y);
        M5.Lcd.print(i == selectedIndex ? "> " : "  ");
        M5.Lcd.print(MENU_LABELS[i]);
    }

    drawControlHint();
}

void GUI::drawTutorial(int step, const Player &player, const WallManager &wallManager) {
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);

    if (step == 0) {
        M5.Lcd.drawString("MOVE WITH TOUCH", screenWidth / 2, 6);
    } else if (step == 1) {
        drawWalls(wallManager);
        M5.Lcd.drawString("HIT WALL = GAME OVER", screenWidth / 2, 6);
    } else {
        drawWalls(wallManager);
        M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
        M5.Lcd.drawString("GAME OVER EXAMPLE", screenWidth / 2, 6);
        M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    }
    drawPlayer(player);

    M5.Lcd.setTextDatum(BC_DATUM);
    M5.Lcd.setTextColor(TFT_DARKGREY, TFT_BLACK);
    M5.Lcd.drawString(step == 2 ? "[OK/C]TITLE" : "[OK/C]SKIP", screenWidth / 2, screenHeight - 1);
}

void GUI::drawSettings(Sensitivity current) {
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.drawString("SENSITIVITY", screenWidth / 2, 6);

    const char *labels[3] = {"LOW", "MID", "HIGH"};
    int currentIndex = static_cast<int>(current);
    for (int i = 0; i < 3; i++) {
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.setTextColor(i == currentIndex ? TFT_YELLOW : TFT_WHITE, TFT_BLACK);
        M5.Lcd.drawString(labels[i], (i + 1) * screenWidth / 4, screenHeight / 2);
    }

    drawControlHint();
}

void GUI::drawDifficultySelect(int level) {
    // G38/G39配線不良の暫定対策: 上下選択に合わせて縦並び表示にする(基板修正版が届き次第、横並びに戻す)
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    M5.Lcd.drawString("SELECT LEVEL", screenWidth / 2, 6);

    for (int i = 1; i <= 3; i++) {
        M5.Lcd.setTextDatum(MC_DATUM);
        M5.Lcd.setTextColor(i == level ? TFT_YELLOW : TFT_WHITE, TFT_BLACK);
        char buf[8];
        snprintf(buf, sizeof(buf), "Lv.%d", i);
        M5.Lcd.drawString(buf, screenWidth / 2, screenHeight / 4 + (i - 1) * (screenHeight / 4));
    }

    drawControlHint();
}

void GUI::drawPlaying(const Player &player, const WallManager &wallManager, unsigned long survivalTimeMs) {
    M5.Lcd.fillScreen(TFT_BLACK);

    M5.Lcd.setTextDatum(TL_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    char buf[16];
    snprintf(buf, sizeof(buf), "%lu.%01lus", survivalTimeMs / 1000, (survivalTimeMs % 1000) / 100);
    M5.Lcd.setCursor(2, 2);
    M5.Lcd.print(buf);

    drawWalls(wallManager);
    drawPlayer(player);
}

void GUI::drawGameOver(unsigned long survivalTimeMs) {
    M5.Lcd.fillScreen(TFT_BLACK);
    M5.Lcd.setTextDatum(TC_DATUM);
    M5.Lcd.setTextSize(1);
    M5.Lcd.setTextColor(TFT_RED, TFT_BLACK);
    M5.Lcd.drawString("GAME OVER", screenWidth / 2, 30);

    M5.Lcd.setTextColor(TFT_WHITE, TFT_BLACK);
    char buf[24];
    snprintf(buf, sizeof(buf), "Time: %lu.%01lus", survivalTimeMs / 1000, (survivalTimeMs % 1000) / 100);
    M5.Lcd.drawString(buf, screenWidth / 2, 60);

    M5.Lcd.setTextDatum(BC_DATUM);
    M5.Lcd.setTextColor(TFT_DARKGREY, TFT_BLACK);
    M5.Lcd.drawString("[OK/C]TITLE", screenWidth / 2, screenHeight - 1);
}
