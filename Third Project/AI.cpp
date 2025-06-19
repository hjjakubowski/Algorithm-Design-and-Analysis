#include "AI.hpp"
#include "zorbist.hpp"

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
inline bool move_capture_first(const Board& board, const Move& m, PieceColor color) {
    PieceColor enemy = (color == WHITE) ? BLACK : WHITE;
    for (int t = 0; t < 6; ++t)
        if (board.pieces[enemy][t] & (1ULL << m.to))
            return true;
    return false;
}
void order_moves(Board& board, std::vector<Move>& moves, PieceColor color) {
    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        return move_capture_first(board, a, color) > move_capture_first(board, b, color);
        });
}

int evaluate_heuristics(const Board& board) {

    if (board.isCheckmate(BLACK))
        return 100000 - board.halfmoveClock; 
    if (board.isCheckmate(WHITE))
        return -100000 + board.halfmoveClock;


    if (board.isStalemate(BLACK) || board.isStalemate(WHITE) || board.halfmoveClock >= 100)
        return 0;

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
    
    int wq = -1, wk = -1, bk = -1;
    int whiteCount = 0, blackCount = 0;

    if (whiteCount == 2 && blackCount == 1 && wq != -1 && wk != -1 && bk != -1) {
        int blackX = bk % 8, blackY = bk / 8;

 
        int edge_dist = std::min({ blackX, 7 - blackX, blackY, 7 - blackY });

        score += (3 - edge_dist) * 500; // Potê¿ny bonus za zagonienie do rogu
        int kk_dist = std::abs((wk % 8) - blackX) + std::abs((wk / 8) - blackY);
        score += (8 - kk_dist) * 100;

     
        int qk_dist = std::abs((wq % 8) - blackX) + std::abs((wq / 8) - blackY);
        score += (8 - qk_dist) * 10;

    
        score -= board.halfmoveClock * 50; // AI szybciej d¹¿y do mata

 
        if ((edge_dist == 0) && (kk_dist <= 2)) {
            score += 90000;
        }
    }


    return score;
}
int quiescence(Board& board, int alpha, int beta, PieceColor color) {
    int stand_pat = evaluate_heuristics(board);
    if (stand_pat >= beta)
        return beta;
    if (alpha < stand_pat)
        alpha = stand_pat;

    auto moves = board.generateAllLegalMoves(color);
    std::vector<Move> tactical_moves;
    PieceColor enemy = (color == WHITE) ? BLACK : WHITE;
    for (const Move& m : moves) {
        bool isCapture = false;
        for (int t = 0; t < 6; ++t)
            if (board.pieces[enemy][t] & (1ULL << m.to))
                isCapture = true;
        if (isCapture || m.promoPiece != NONE_TYPE) {
            tactical_moves.push_back(m);
        }
    }

    for (const Move& m : tactical_moves) {
        board.makeMove(m, color);
        int score = -quiescence(board, -beta, -alpha, enemy);
        board.undoMove();

        if (score >= beta)
            return beta;
        if (score > alpha)
            alpha = score;
    }
    return alpha;
}


int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, PieceColor color) {
    PieceColor enemy = (color == WHITE) ? BLACK : WHITE;

    uint64_t key = hash_board(board);
    auto it = transTable.find(key);
    if (it != transTable.end() && it->second.depth >= depth) {
        return it->second.value;
    }

    if (depth == 0 || board.isCheckmate(color) || board.isStalemate(color))
        return quiescence(board, alpha, beta, color);

    auto moves = board.generateAllLegalMoves(color);

    order_moves(board, moves, color);

    int bestEval = maximizingPlayer ? -1000000 : 1000000;

    if (maximizingPlayer) {
        for (const Move& m : moves) {
            board.makeMove(m, color);
            int eval = minimax(board, depth - 1, alpha, beta, false, enemy);
            board.undoMove();
            bestEval = std::max(bestEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break;
        }
    }
    else {
        for (const Move& m : moves) {
            board.makeMove(m, color);
            int eval = minimax(board, depth - 1, alpha, beta, true, enemy);
            board.undoMove();
            bestEval = std::min(bestEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break;
        }
    }

    transTable[key] = { depth, bestEval };

    return bestEval;
}



Move get_best_move_AI(Board& board, int depth, PieceColor color) {
    auto moves = board.generateAllLegalMoves(color);
    order_moves(board, moves, color);

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
