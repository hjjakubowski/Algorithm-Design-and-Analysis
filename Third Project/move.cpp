#include "move.hpp"
#include "piece.hpp"
#include <cassert>


Move Move::fromString(const std::string& str) {
    assert(str.length() >= 4);
    int fx = str[0] - 'a';
    int fy = str[1] - '1';
    int tx = str[2] - 'a';
    int ty = str[3] - '1';
    PieceType promo = NONE_TYPE;

    if (str.length() == 5) {
        char promoChar = str[4];
        switch (promoChar) {
        case 'q': promo = QUEEN;  break;
        case 'r': promo = ROOK;   break;
        case 'n': promo = KNIGHT; break;
        case 'b': promo = BISHOP; break;

        }
    }
    return Move(sq(fy, fx), sq(ty, tx), promo);
}
