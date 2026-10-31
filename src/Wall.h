#pragma once

// 壁1本分のデータ(上下1組で1つの開口を形成する)
struct Wall {
    int x;          // 壁の左端x座標
    int gapY;        // 開口の上端y座標
    int gapHeight;    // 開口の高さ
    int thickness;    // 壁の横幅
    bool active;
};
