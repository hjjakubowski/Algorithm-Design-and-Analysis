#pragma once
#include <string>
#include "piece.hpp"

// Ruch: from/to na bitboardzie to indeks 0..63
struct Move {
    int from; // 0..63
    int to;   // 0..63
    PieceType promoPiece; // do promocji pionka

    Move() : from(0), to(0), promoPiece(NONE_TYPE) {}
    Move(int f, int t, PieceType promo = NONE_TYPE)
        : from(f), to(t), promoPiece(promo) {
    }

    static Move fromString(const std::string& str); // np. "e2e4", "a7a8q"
};
