#include "AI.hpp"


const int pawn_table[64] = {
      0,  0,  0,  0,  0,  0,  0,  0,
     50, 50, 50, 50, 50, 50, 50, 50,
     10, 10, 20, 30, 30, 20, 10, 10,
      5,  5, 10, 25, 25, 10,  5,  5,
      0,  0,  0, 20, 20,  0,  0,  0,
      5, -5,-10,  0,  0,-10, -5,  5,
      5, 10, 10,-20,-20, 10, 10,  5,
      0,  0,  0,  0,  0,  0,  0,  0
};

const int knight_table[64] = {
   -50,-40,-30,-30,-30,-30,-40,-50,
   -40,-20,  0,  5,  5,  0,-20,-40,
   -30,  5, 10, 15, 15, 10,  5,-30,
   -30,  0, 15, 20, 20, 15,  0,-30,
   -30,  5, 15, 20, 20, 15,  5,-30,
   -30,  0, 10, 15, 15, 10,  0,-30,
   -40,-20,  0,  0,  0,  0,-20,-40,
   -50,-40,-30,-30,-30,-30,-40,-50
};

const int bishop_table[64] = {
   -20,-10,-10,-10,-10,-10,-10,-20,
   -10,  5,  0,  0,  0,  0,  5,-10,
   -10, 10, 10, 10, 10, 10, 10,-10,
   -10,  0, 10, 10, 10, 10,  0,-10,
   -10,  5,  5, 10, 10,  5,  5,-10,
   -10,  0,  5, 10, 10,  5,  0,-10,
   -10,  0,  0,  0,  0,  0,  0,-10,
   -20,-10,-10,-10,-10,-10,-10,-20
};

const int rook_table[64] = {
     0,  0,  0,  5,  5,  0,  0,  0,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
     5, 10, 10, 10, 10, 10, 10,  5,
     0,  0,  0,  0,  0,  0,  0,  0
};

const int queen_table[64] = {
   -20,-10,-10, -5, -5,-10,-10,-20,
   -10,  0,  0,  0,  0,  0,  0,-10,
   -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
     0,  0,  5,  5,  5,  5,  0, -5,
   -10,  5,  5,  5,  5,  5,  0,-10,
   -10,  0,  5,  0,  0,  0,  0,-10,
   -20,-10,-10, -5, -5,-10,-10,-20
};

const int king_table[64] = {
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -30,-40,-40,-50,-50,-40,-40,-30,
   -20,-30,-30,-40,-40,-30,-30,-20,
   -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};

int piece_value(PieceType pt) {
    switch (pt) {
    case PAWN:   return 100;
    case KNIGHT: return 320;
    case BISHOP: return 330;
    case ROOK:   return 500;
    case QUEEN:  return 900;
    case KING:   return 10000;
    default:     return 0;
    }
}

int positional_bonus(PieceType pt, PieceColor color, int sq) {
    int r = color == WHITE ? sq : (56 + (sq % 8) - 8 * (sq / 8));
    switch (pt) {
    case PAWN:   return pawn_table[r];
    case KNIGHT: return knight_table[r];
    case BISHOP: return bishop_table[r];
    case ROOK:   return rook_table[r];
    case QUEEN:  return queen_table[r];
    case KING:   return king_table[r];
    default:     return 0;
    }
}

int evaluate_heuristics(const Board& board) {
    int score = 0;
    for (int color = 0; color <= 1; ++color) {
        for (int pt = 0; pt <= 5; ++pt) {
            Bitboard bb = board.pieces[color][pt];
            while (bb) {
                int sq = bit_scan_forward(bb);
                int val = piece_value((PieceType)pt) + positional_bonus((PieceType)pt, (PieceColor)color, sq);
                score += (color == WHITE) ? val : -val;
                bb &= bb - 1;
            }
        }
    }
    return score;
}

int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, PieceColor color) {
    PieceColor enemy = (color == WHITE) ? BLACK : WHITE;
    if (depth == 0 || board.isCheckmate(color) || board.isStalemate(color))
        return evaluate_heuristics(board);

    auto moves = board.generateAllMoves(color);
    if (moves.empty())
        return evaluate_heuristics(board);

    if (maximizingPlayer) {
        int maxEval = -1000000;
        for (const Move& m : moves) {
            board.makeMove(m, color);
            int eval = minimax(board, depth - 1, alpha, beta, false, enemy);
            board.undoMove();
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
        return maxEval;
    }
    else {
        int minEval = 1000000;
        for (const Move& m : moves) {
            board.makeMove(m, color);
            int eval = minimax(board, depth - 1, alpha, beta, true, enemy);
            board.undoMove();
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
        return minEval;
    }
}

Move get_best_move_AI(Board& board, int depth, PieceColor color) {
    auto moves = board.generateAllMoves(color);
    Move bestMove;
    int bestEval = (color == WHITE) ? -1000000 : 1000000;
    for (const Move& m : moves) {
        board.makeMove(m, color);
        int eval = minimax(board, depth - 1, -1000000, 1000000, color == BLACK, (color == WHITE) ? BLACK : WHITE);
        board.undoMove();
        if (color == WHITE) {
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
