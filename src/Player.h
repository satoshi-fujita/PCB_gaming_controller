#pragma once

// 自機の座標・サイズの保持と移動(design.md 3.2節)
class Player {
public:
    Player();

    void reset(int size);
    void move(bool up, bool down, bool left, bool right);

    int x() const;
    int y() const;
    int size() const;

private:
    static const int MOVE_STEP = 2;
    static const int MIN_X = 12;
    static const int MAX_X = 32;
    static const int SCREEN_HEIGHT = 128;

    int posX;
    int posY;
    int playerSize;

    int clamp(int value, int lo, int hi) const;
};
