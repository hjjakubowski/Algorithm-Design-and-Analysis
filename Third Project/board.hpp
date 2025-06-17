#pragma once
#include <array>
#include <vector>
#include <utility>
#include "piece.hpp"
#include "move.hpp"

class Board {
public:
    std::array<std::array<uint8_t, 8>, 8> squares;

    struct HistoryEntry {
        std::array<std::array<uint8_t, 8>, 8> state;
        Move move;
    };
    std::vector<HistoryEntry> history;

    Board();
    void setInitial();
    void print() const;

    bool isLegalMove(int fromY, int fromX, int toY, int toX, uint8_t promo = 0) const;
    bool makeMove(int fromY, int fromX, int toY, int toX, uint8_t promo = 0);
    void undoMove();

    bool isCheck(PieceColor color) const;
    bool isCheckmate(PieceColor color) const;
    bool isStalemate(PieceColor color) const;

    std::vector<Move> legalPawnMoves(int y, int x) const;
    std::vector<Move> legalKnightMoves(int y, int x) const;
    std::vector<Move> legalBishopMoves(int y, int x) const;
    std::vector<Move> legalRookMoves(int y, int x) const;
    std::vector<Move> legalQueenMoves(int y, int x) const;
    std::vector<Move> legalKingMoves(int y, int x) const;

    std::vector<Move> generateAllMoves(PieceColor color) const;
    std::pair<int, int> findKing(PieceColor color) const;
};
