#include "AI.hpp"
#include "zorbist.hpp"
#include <algorithm>
#include <vector>

std::unordered_map<uint64_t, TransTableEntry> transTable;

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


int evaluate_heuristics(const Board& board) {
    // Specjalne przypadki – wygrana, przegrana, remis
    if (board.isCheckmate(BLACK))
        return 100000 - board.halfmoveClock;
    if (board.isCheckmate(WHITE))
        return -100000 + board.halfmoveClock;

    if (board.isStalemate(BLACK) || board.isStalemate(WHITE) || board.halfmoveClock >= 100)
        return -5000; // Z£O: remis jest z³y, karzemy remis (mo¿esz zmieniaæ t¹ wartoœæ)

    if (board.isInsufficientMaterial())
        return -8000; // Bardzo z³a ocena, jeszcze wiêksza kara za wyczyszczenie planszy

    // Suma wartoœci materia³u, premii pozycyjnych i aktywnoœci
    int score = 0;

    int white_material = 0, black_material = 0;
    int white_pawns = 0, black_pawns = 0;
    int white_rooks = 0, black_rooks = 0;
    int white_queens = 0, black_queens = 0;
    int white_minor = 0, black_minor = 0; // skoczki+biskupy

    // Liczenie materia³u, pozycji i aktywnoœci
    for (int color = 0; color <= 1; ++color) {
        for (int pt = 0; pt <= 5; ++pt) {
            Bitboard bb = board.pieces[color][pt];
            while (bb) {
                int sq = bit_scan_forward(bb);
                int val = piece_value((PieceType)pt) + positional_bonus((PieceType)pt, (PieceColor)color, sq);

                // Aktywnoœæ: figury na 3-7 ranku dostaj¹ bonus
                int y = sq / 8;
                if ((color == WHITE && y >= 3) || (color == BLACK && y <= 4))
                    val += 15; // premiuj za wejœcie w po³owê przeciwnika

                // premiuj mobilnoœæ hetmana, wie¿y i goñca po otwartej linii
                if ((pt == ROOK || pt == QUEEN) && ((color == WHITE && y >= 4) || (color == BLACK && y <= 3)))
                    val += 10;

                score += (color == WHITE) ? val : -val;

                // Liczenie materia³u
                if (color == WHITE) {
                    if (pt == PAWN) white_pawns++;
                    if (pt == ROOK) white_rooks++;
                    if (pt == QUEEN) white_queens++;
                    if (pt == KNIGHT || pt == BISHOP) white_minor++;
                    white_material += piece_value((PieceType)pt);
                }
                else {
                    if (pt == PAWN) black_pawns++;
                    if (pt == ROOK) black_rooks++;
                    if (pt == QUEEN) black_queens++;
                    if (pt == KNIGHT || pt == BISHOP) black_minor++;
                    black_material += piece_value((PieceType)pt);
                }
                bb &= bb - 1;
            }
        }
    }

    // Kara za go³e króle – absolutny remis
    if (white_material == 0 && black_material == 0)
        score -= 50000;

    // Endgame logic: premiuj matowanie, karz za wyczyszczenie planszy
    // Jeœli masz hetmana/hetmana+wie¿ê a przeciwnik sam król, bonus za matowanie
    if (white_queens >= 1 && black_material == 0 && black_pawns == 0 && black_rooks == 0 && black_minor == 0)
        score += 20000;
    if (black_queens >= 1 && white_material == 0 && white_pawns == 0 && white_rooks == 0 && white_minor == 0)
        score -= 20000;

    // Aktywnoœæ pionków – premiuj pionki które przesz³y po³owê planszy
    for (int sq = 0; sq < 64; ++sq) {
        if (board.pieces[WHITE][PAWN] & (1ULL << sq)) {
            int y = sq / 8;
            if (y >= 4) score += 20 * (y - 3); // im dalej tym lepiej
        }
        if (board.pieces[BLACK][PAWN] & (1ULL << sq)) {
            int y = sq / 8;
            if (y <= 3) score -= 20 * (4 - y);
        }
    }

    // Aktywnoœæ wie¿ na otwartych liniach – bonus
    for (int file = 0; file < 8; ++file) {
        bool white_rook = false, black_rook = false, any_pawn = false;
        for (int rank = 0; rank < 8; ++rank) {
            int sq = rank * 8 + file;
            if (board.pieces[WHITE][ROOK] & (1ULL << sq)) white_rook = true;
            if (board.pieces[BLACK][ROOK] & (1ULL << sq)) black_rook = true;
            if (board.pieces[WHITE][PAWN] & (1ULL << sq) || board.pieces[BLACK][PAWN] & (1ULL << sq)) any_pawn = true;
        }
        if (white_rook && !any_pawn) score += 30;
        if (black_rook && !any_pawn) score -= 30;
    }

    // Premia za rozwoj: jeœli wszystkie figury wysz³y z pierwszego rzêdu – bonus
    int white_dev = 0, black_dev = 0;
    for (int pt = 1; pt <= 5; ++pt) {
        Bitboard bb = board.pieces[WHITE][pt];
        while (bb) {
            int sq = bit_scan_forward(bb);
            if (sq / 8 > 0) white_dev++;
            bb &= bb - 1;
        }
        bb = board.pieces[BLACK][pt];
        while (bb) {
            int sq = bit_scan_forward(bb);
            if (sq / 8 < 7) black_dev++;
            bb &= bb - 1;
        }
    }
    score += 5 * white_dev - 5 * black_dev;

    // Kara za pasywnoœæ (figurki na linii pocz¹tkowej, nie ruszane) – im d³u¿ej, tym gorzej
    // (mo¿esz rozwin¹æ o w³asn¹ logikê)

    // Premia za bezpieczeñstwo króla (np. pionki przed królem)
    int white_king_sq = board.findKing(WHITE), black_king_sq = board.findKing(BLACK);
    if (white_king_sq >= 0) {
        int file = white_king_sq % 8;
        int rank = white_king_sq / 8;
        // pionki przed królem
        for (int dx = -1; dx <= 1; ++dx) {
            int x = file + dx, y = rank + 1;
            if (x >= 0 && x < 8 && y < 8)
                if (board.pieces[WHITE][PAWN] & (1ULL << (y * 8 + x)))
                    score += 8;
        }
    }
    if (black_king_sq >= 0) {
        int file = black_king_sq % 8;
        int rank = black_king_sq / 8;
        // pionki przed królem
        for (int dx = -1; dx <= 1; ++dx) {
            int x = file + dx, y = rank - 1;
            if (x >= 0 && x < 8 && y >= 0)
                if (board.pieces[BLACK][PAWN] & (1ULL << (y * 8 + x)))
                    score -= 8;
        }
    }

    // Kara za powtórzenie pozycji (tu uproszczenie – mo¿esz rozbudowaæ)
    if (board.halfmoveClock >= 50)
        score -= 2500;

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

void order_moves(Board& board, std::vector<Move>& moves, PieceColor color) {
    // Capture first
    auto move_capture_first = [&](const Move& m) {
        PieceColor enemy = (color == WHITE) ? BLACK : WHITE;
        for (int t = 0; t < 6; ++t)
            if (board.pieces[enemy][t] & (1ULL << m.to))
                return true;
        return false;
        };
    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        return move_capture_first(a) > move_capture_first(b);
        });
}

int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, PieceColor color) {
    PieceColor enemy = (color == WHITE) ? BLACK : WHITE;

    uint64_t key = hash_board(board);
    auto it = transTable.find(key);
    // ODCZYT: u¿ywaj TYLKO gdy wpis jest dla >= tej g³êbokoœci!
    if (it != transTable.end() && it->second.depth >= depth) {
        return it->second.value;
    }

    if (depth == 0 || board.isCheckmate(color) || board.isStalemate(color) || board.isInsufficientMaterial())
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

    // ZAPIS: nadpisuj TYLKO jeœli nie by³o wpisu lub obecny jest dla mniejszej g³êbokoœci
    if (it == transTable.end() || it->second.depth < depth) {
        transTable[key] = { depth, bestEval };
    }
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

