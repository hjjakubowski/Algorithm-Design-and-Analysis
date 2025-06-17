#pragma once
#include "board.hpp"
#include "move.hpp"
#include <algorithm>

extern const int pawn_table[8][8];
extern const int knight_table[8][8];
extern const int bishop_table[8][8];
extern const int rook_table[8][8];
extern const int queen_table[8][8];
extern const int king_table[8][8];

int piece_value(PieceType pt);
int positional_bonus(PieceType pt, PieceColor color, int y, int x);
int evaluate_heuristics(const Board& board);
int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, PieceColor color);
Move get_best_move_AI(Board& board, int depth, PieceColor color);
