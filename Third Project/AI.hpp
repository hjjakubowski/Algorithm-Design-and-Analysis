#pragma once
#include "board.hpp"
#include "move.hpp"
#include <algorithm>
#include <unordered_map>
#include <cstdint>
#include <vector>

void init_zobrist();
uint64_t zobrist_hash(const Board& board);

struct TransTableEntry {
    int depth;
    int value;
};
extern std::unordered_map<uint64_t, TransTableEntry> transTable;

extern const int pawn_table[8][8];
extern const int knight_table[8][8];
extern const int bishop_table[8][8];
extern const int rook_table[8][8];
extern const int queen_table[8][8];
extern const int king_table[8][8];

int piece_value(PieceType pt);
int positional_bonus(PieceType pt, PieceColor color, int y, int x);
int evaluate_board(const Board& board, PieceColor color, const std::vector<Move>& moveHistory, int repetitionPenalty = 1000);
int quick_move_score(const Board& board, const Move& move, PieceColor color);
struct ScoredMove {
    Move move;
    int score;
};
std::vector<ScoredMove> score_and_sort_moves(Board& board, PieceColor color, PieceColor enemy);
int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, PieceColor color, std::vector<Move>& moveHistory);
Move get_best_move_AI(Board& board, int depth, PieceColor color);
bool is_null_move(const std::vector<Move>& history, const Move& current);

