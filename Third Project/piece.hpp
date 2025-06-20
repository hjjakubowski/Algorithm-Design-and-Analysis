#pragma once
#include <cstdint>
#include <string>

enum PieceType : uint8_t {
    NONE = 0b00000000,
    PAWN = 0b00000100,
    ROOK = 0b00001100,
    KNIGHT = 0b00011100,
    BISHOP = 0b00111100,
    QUEEN = 0b01111100,
    KING = 0b11111100,
};

enum PieceColor : uint8_t {
    EMPTY = 0b00,
    WHITE = 0b10,
    BLACK = 0b01
};

// Sklejenie typu i koloru w jeden bajt:
inline uint8_t makePiece(PieceType type, PieceColor color) {
    return (type | color);
}

inline PieceType getPieceType(uint8_t piece) {
    return static_cast<PieceType>(piece & 0b11111100);
}

inline PieceColor getPieceColor(uint8_t piece) {
    return static_cast<PieceColor>(piece & 0b11);
}

// Zamiana na znak (np. do FEN, do wyswietlania)
char pieceToChar(uint8_t piece);