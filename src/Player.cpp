#include "Player.h"

Player::Player() : posX(MIN_X), posY(SCREEN_HEIGHT / 2), playerSize(10) {}

void Player::reset(int size) {
    playerSize = size;
    posX = MIN_X;
    posY = SCREEN_HEIGHT / 2;
}

int Player::clamp(int value, int lo, int hi) const {
    if (value < lo) return lo;
    if (value > hi) return hi;
    return value;
}

void Player::move(bool up, bool down, bool left, bool right) {
    if (up) posY -= MOVE_STEP;
    if (down) posY += MOVE_STEP;
    if (left) posX -= MOVE_STEP;
    if (right) posX += MOVE_STEP;

    posX = clamp(posX, MIN_X, MAX_X);
    posY = clamp(posY, playerSize, SCREEN_HEIGHT - playerSize);
}

int Player::x() const { return posX; }
int Player::y() const { return posY; }
int Player::size() const { return playerSize; }
