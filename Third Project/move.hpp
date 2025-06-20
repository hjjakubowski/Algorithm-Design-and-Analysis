#pragma once
#include <string>

struct Move {
    int fromY, fromX, toY, toX;
    uint8_t promoPiece;

    Move() : fromY(0), fromX(0), toY(0), toX(0), promoPiece(0) {}
    Move(int fy, int fx, int ty, int tx, uint8_t promo = 0)
        : fromY(fy), fromX(fx), toY(ty), toX(tx), promoPiece(promo) {
    }

    static Move fromString(const std::string& str);
};