#pragma once
#include <intrin.h>
#include <cstdint>

enum PieceType : uint8_t {
    PAWN = 0,
    KNIGHT = 1,
    BISHOP = 2,
    ROOK = 3,
    QUEEN = 4,
    KING = 5,
    NONE_TYPE = 6
};

enum PieceColor : uint8_t {
    WHITE = 0,
    BLACK = 1,
    NO_COLOR = 2
};

// Bitboard helpers
using Bitboard = uint64_t;

inline int sq(int y, int x) { return y * 8 + x; } // indeks pola na 0..63
inline int rank_of(int sq) { return sq / 8; }
inline int file_of(int sq) { return sq % 8; }

// Zamiana (PieceType, PieceColor) na znak:
inline char pieceToChar(PieceType pt, PieceColor color) {
    static const char chars[2][6] = {
        {'P', 'N', 'B', 'R', 'Q', 'K'}, // White
        {'p', 'n', 'b', 'r', 'q', 'k'}  // Black
    };
    if (pt > KING || color > BLACK) return '.';
    return chars[color][pt];
}

inline int bit_scan_forward(Bitboard b) {
    unsigned long idx;
    _BitScanForward64(&idx, b);
    return static_cast<int>(idx);
}
