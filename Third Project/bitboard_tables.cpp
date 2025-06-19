#include "bitboard_tables.hpp"

Bitboard knight_attacks[64];
Bitboard king_attacks[64];

void init_knight_attacks() {
    for (int sq = 0; sq < 64; ++sq) {
        int y = sq / 8, x = sq % 8;
        knight_attacks[sq] = 0;
        static const int dy[] = { 2, 2, 1, 1, -1, -1, -2, -2 };
        static const int dx[] = { 1, -1, 2, -2, 2, -2, 1, -1 };
        for (int d = 0; d < 8; ++d) {
            int ny = y + dy[d], nx = x + dx[d];
            if (ny >= 0 && ny < 8 && nx >= 0 && nx < 8)
                knight_attacks[sq] |= (1ULL << (ny * 8 + nx));
        }
    }
}
void init_king_attacks() {
    for (int sq = 0; sq < 64; ++sq) {
        int y = sq / 8, x = sq % 8;
        king_attacks[sq] = 0;
        for (int dy = -1; dy <= 1; ++dy)
            for (int dx = -1; dx <= 1; ++dx) {
                if (dy == 0 && dx == 0) continue;
                int ny = y + dy, nx = x + dx;
                if (ny >= 0 && ny < 8 && nx >= 0 && nx < 8)
                    king_attacks[sq] |= (1ULL << (ny * 8 + nx));
            }
    }
}
