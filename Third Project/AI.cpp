#include "ai.hpp"

int piece_value(PieceType pt) {
    switch (pt) {
    case PAWN:   return 100;
    case KNIGHT: return 320;
    case BISHOP: return 330;
    case ROOK:   return 500;
    case QUEEN:  return 900;
    case KING:   return 10000;
    default: return 0;
    }
}

int evaluate_heuristics(const Board& board) {
    int score = 0;
    for (int y = 0; y < 8; ++y)
        for (int x = 0; x < 8; ++x) {
            uint8_t piece = board.squares[y][x];
            if (piece == NONE) continue;
            int value = piece_value(getPieceType(piece));
            if (getPieceColor(piece) == WHITE)
                score += value;
            else if (getPieceColor(piece) == BLACK)
                score -= value;
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
            board.makeMove(m.fromY, m.fromX, m.toY, m.toX, m.promoPiece);
            int eval = minimax(board, depth - 1, alpha, beta, false, enemy);
            board.undoMove();
            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break;
        }
        return maxEval;
    }
    else {
        int minEval = 1000000;
        for (const Move& m : moves) {
            board.makeMove(m.fromY, m.fromX, m.toY, m.toX, m.promoPiece);
            int eval = minimax(board, depth - 1, alpha, beta, true, enemy);
            board.undoMove();
            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break;
        }
        return minEval;
    }
}

Move get_best_move_AI(Board& board, int depth, PieceColor color) {
    auto moves = board.generateAllMoves(color);
    Move bestMove;
    int bestEval = (color == WHITE) ? -1000000 : 1000000;

    for (const Move& m : moves) {
        board.makeMove(m.fromY, m.fromX, m.toY, m.toX, m.promoPiece);
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
