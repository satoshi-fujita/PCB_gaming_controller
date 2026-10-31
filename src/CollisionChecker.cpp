#include "CollisionChecker.h"

namespace {
    const int SCREEN_HEIGHT = 128;
}

bool CollisionChecker::overlaps(int ax1, int ay1, int ax2, int ay2,
                                 int bx1, int by1, int bx2, int by2) {
    return ax1 < bx2 && ax2 > bx1 && ay1 < by2 && ay2 > by1;
}

bool CollisionChecker::check(const Player &player, const WallManager &wallManager) {
    int half = player.size() / 2;
    int px1 = player.x() - half;
    int px2 = player.x() + half;
    int py1 = player.y() - half;
    int py2 = player.y() + half;

    const Wall *walls = wallManager.walls();
    for (int i = 0; i < wallManager.wallCount(); i++) {
        if (!walls[i].active) continue;

        int wx1 = walls[i].x;
        int wx2 = walls[i].x + walls[i].thickness;

        // 上側の壁: 0 〜 gapY
        if (overlaps(px1, py1, px2, py2, wx1, 0, wx2, walls[i].gapY)) {
            return true;
        }
        // 下側の壁: gapY+gapHeight 〜 画面下端
        int gapBottom = walls[i].gapY + walls[i].gapHeight;
        if (overlaps(px1, py1, px2, py2, wx1, gapBottom, wx2, SCREEN_HEIGHT)) {
            return true;
        }
    }
    return false;
}
