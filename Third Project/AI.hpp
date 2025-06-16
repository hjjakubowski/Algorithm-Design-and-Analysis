#pragma once

#include "chess.hpp"
#include <algorithm>

using namespace chess;

int piece_value(PieceType pt) {
    switch (pt.internal()) {
    case PieceType::PAWN:   return 100;
    case PieceType::KNIGHT: return 320;
    case PieceType::BISHOP: return 330;
    case PieceType::ROOK:   return 500;
    case PieceType::QUEEN:  return 900;
    case PieceType::KING:   return 10000;
    default: return 0;
    }
}

int evaluate_heuristics(const Board& board) {
    int score = 0;
    for (int sq = 0; sq < 64; ++sq) {
        Piece p = board.at(Square(sq));
        if (p == Piece::NONE) continue;
        int value = piece_value(p.type());
        if (p.color() == Color::WHITE) score += value;
        else if (p.color() == Color::BLACK) score -= value;
    }
    return score;
}

int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer) {
    auto [reason, result] = board.isGameOver();
    if (depth == 0 || reason != GameResultReason::NONE)
        return evaluate_heuristics(board);

    Movelist moves;
    movegen::legalmoves(moves, board);

    if (maximizingPlayer) {
        int maxEval = -1000000;
        for (const Move& m : moves) {
            board.makeMove(m);
            int eval = minimax(board, depth - 1, alpha, beta, false);
            board.unmakeMove(m);
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break; // beta cutoff
        }
        return maxEval;
    }
    else {
        int minEval = 1000000;
        for (const Move& m : moves) {
            board.makeMove(m);
            int eval = minimax(board, depth - 1, alpha, beta, true);
            board.unmakeMove(m);
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break; // alpha cutoff
        }
        return minEval;
    }
}

Move get_best_move(Board& board, int depth) {
    Movelist moves;
    movegen::legalmoves(moves, board);
    Move bestMove = Move(Move::NO_MOVE);
    int bestEval = (board.sideToMove() == Color::WHITE) ? -1000000 : 1000000;

    for (const Move& m : moves) {
        board.makeMove(m);
        int eval = minimax(board, depth - 1, -1000000, 1000000, board.sideToMove() == Color::BLACK);
        board.unmakeMove(m);

        if (board.sideToMove() == Color::WHITE) {
            if (eval > bestEval) {
                bestEval = eval;
                bestMove = m;
            }
        }
        else {
            if (eval < bestEval) {
                bestEval = eval;
                bestMove = m;
            }
        }
    }
    return bestMove;
}
