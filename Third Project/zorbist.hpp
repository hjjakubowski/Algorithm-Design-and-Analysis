#pragma once
#include "board.hpp"     // Tu masz pe³n¹ deklaracjê Board!
#include <random>
#include <cstdint>

// Globalna tablica hashuj¹ca [kolor][typ][pole]
static uint64_t zobrist_table[2][6][64];

inline void init_zobrist() {
    std::mt19937_64 rng(202406);
    for (int c = 0; c < 2; ++c)
        for (int t = 0; t < 6; ++t)
            for (int sq = 0; sq < 64; ++sq)
                zobrist_table[c][t][sq] = rng();
}

inline uint64_t hash_board(const Board& board) {
    uint64_t h = 0;
    for (int c = 0; c < 2; ++c)
        for (int t = 0; t < 6; ++t) {
            Bitboard bb = board.pieces[c][t];
            while (bb) {
                int sq = bit_scan_forward(bb);
                h ^= zobrist_table[c][t][sq];
                bb &= bb - 1;
            }
        }
    return h;
}
