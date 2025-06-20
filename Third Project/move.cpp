#include "move.hpp"
#include "piece.hpp"

Move Move::fromString(const std::string& str) {
    int fx = str[0] - 'a';
    int fy = str[1] - '1';
    int tx = str[2] - 'a';
    int ty = str[3] - '1';
    uint8_t promo = 0;
    if (str.size() == 5) {
        switch (str[4]) {
        case 'h': promo = QUEEN; break;
        case 'w': promo = ROOK; break;
        case 'g': promo = KNIGHT; break;
        case 's': promo = BISHOP; break;
        }
    }
    return Move(fy, fx, ty, tx, promo);
}
