#pragma once
#include "board.hpp"
#include "move.hpp"
#include <algorithm>

int piece_value(PieceType pt);
int evaluate_heuristics(const Board& board);
int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, PieceColor color);
Move get_best_move_AI(Board& board, int depth, PieceColor color);
