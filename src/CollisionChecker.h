#pragma once

#include "Player.h"
#include "WallManager.h"

// 自機と壁群の当たり判定(矩形同士のAABB判定。design.md 3.2節)
class CollisionChecker {
public:
    static bool check(const Player &player, const WallManager &wallManager);

private:
    static bool overlaps(int ax1, int ay1, int ax2, int ay2,
                         int bx1, int by1, int bx2, int by2);
};
