#pragma once
#include "board.hpp"
#include "move.hpp"
#include <algorithm>
#include <vector>
#include <unordered_map>

struct TransTableEntry {
    int depth;
    int value;
};
extern std::unordered_map<uint64_t, TransTableEntry> transTable;

extern const int pawn_table[64];
extern const int knight_table[64];
extern const int bishop_table[64];
extern const int rook_table[64];
extern const int queen_table[64];
extern const int king_table[64];

int piece_value(PieceType pt);
int positional_bonus(PieceType pt, PieceColor color, int sq);
int evaluate_heuristics(const Board& board);
int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, PieceColor color);
Move get_best_move_AI(Board& board, int depth, PieceColor color);
