#pragma once
#include "piece.hpp"
#include "move.hpp"
#include <array>
#include <vector>
#include <cstdint>

struct Board {
    Bitboard pieces[2][6]; 
    Bitboard occupied[2];  
    Bitboard all;          
    int halfmoveClock = 0;

    struct HistoryEntry {
        Bitboard pieces[2][6];
        Bitboard occupied[2];
        Bitboard all;
        Move move;
        int halfmoveClock;
    };
    std::vector<HistoryEntry> history;

    Board();
    void setInitial();    
    void print() const; 

    bool isLegalMove(const Move& m, PieceColor color) const;
    bool makeMove(const Move& m, PieceColor color);
    void undoMove();

    bool isCheck(PieceColor color) const;
    bool isCheckmate(PieceColor color) const;
    bool isStalemate(PieceColor color) const;

    std::vector<Move> generateAllMoves(PieceColor color) const;

    int findKing(PieceColor color) const;
};
