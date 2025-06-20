#include "AI.hpp"
#include <algorithm>
#include <ctime>
#include <unordered_map>
#include <random>
#include <set>
#include <vector>

const int pawn_table[8][8] = {
    {   0,   0,   0,   0,   0,   0,   0,   0 },
    {  50,  50,  50,  50,  50,  50,  50,  50 },
    {  10,  10,  20,  30,  30,  20,  10,  10 },
    {   5,   5,  10,  25,  25,  10,   5,   5 },
    {   0,   0,   0,  20,  20,   0,   0,   0 },
    {   5,  -5, -10,   0,   0, -10,  -5,   5 },
    {   5,  10,  10, -20, -20,  10,  10,   5 },
    {   0,   0,   0,   0,   0,   0,   0,   0 }
};

const int knight_table[8][8] = {
    { -50, -40, -30, -30, -30, -30, -40, -50 },
    { -40, -20,   0,   5,   5,   0, -20, -40 },
    { -30,   5,  10,  15,  15,  10,   5, -30 },
    { -30,   0,  15,  20,  20,  15,   0, -30 },
    { -30,   5,  15,  20,  20,  15,   5, -30 },
    { -30,   0,  10,  15,  15,  10,   0, -30 },
    { -40, -20,   0,   0,   0,   0, -20, -40 },
    { -50, -40, -30, -30, -30, -30, -40, -50 }
};

const int bishop_table[8][8] = {
    { -20, -10, -10, -10, -10, -10, -10, -20 },
    { -10,   5,   0,   0,   0,   0,   5, -10 },
    { -10,  10,  10,  10,  10,  10,  10, -10 },
    { -10,   0,  10,  10,  10,  10,   0, -10 },
    { -10,   5,   5,  10,  10,   5,   5, -10 },
    { -10,   0,   5,  10,  10,   5,   0, -10 },
    { -10,   0,   0,   0,   0,   0,   0, -10 },
    { -20, -10, -10, -10, -10, -10, -10, -20 }
};

const int rook_table[8][8] = {
    {   0,   0,   0,   5,   5,   0,   0,   0 },
    {  -5,   0,   0,   0,   0,   0,   0,  -5 },
    {  -5,   0,   0,   0,   0,   0,   0,  -5 },
    {  -5,   0,   0,   0,   0,   0,   0,  -5 },
    {  -5,   0,   0,   0,   0,   0,   0,  -5 },
    {  -5,   0,   0,   0,   0,   0,   0,  -5 },
    {   5,  10,  10,  10,  10,  10,  10,   5 },
    {   0,   0,   0,   0,   0,   0,   0,   0 }
};

const int queen_table[8][8] = {
    { -20, -10, -10,  -5,  -5, -10, -10, -20 },
    { -10,   0,   0,   0,   0,   0,   0, -10 },
    { -10,   0,   5,   5,   5,   5,   0, -10 },
    {  -5,   0,   5,   5,   5,   5,   0,  -5 },
    {   0,   0,   5,   5,   5,   5,   0,  -5 },
    { -10,   5,   5,   5,   5,   5,   0, -10 },
    { -10,   0,   5,   0,   0,   0,   0, -10 },
    { -20, -10, -10,  -5,  -5, -10, -10, -20 }
};

const int king_table[8][8] = {
    { -30, -40, -40, -50, -50, -40, -40, -30 },
    { -30, -40, -40, -50, -50, -40, -40, -30 },
    { -30, -40, -40, -50, -50, -40, -40, -30 },
    { -30, -40, -40, -50, -50, -40, -40, -30 },
    { -20, -30, -30, -40, -40, -30, -30, -20 },
    { -10, -20, -20, -20, -20, -20, -20, -10 },
    {  20,  20,   0,   0,   0,   0,  20,  20 },
    {  20,  30,  10,   0,   0,  10,  30,  20 }
};

std::unordered_map<uint64_t, TransTableEntry> transTable;
std::mt19937 rng_engine(std::random_device{}());
uint64_t zobrist_table[8][8][13]; // [y][x][figure]
std::unordered_map<uint64_t, int> positionCounts;

// --- Hashowanie i wartoœci ---
void init_zobrist() {
    static bool initialized = false;
    if (initialized) return;
    std::mt19937_64 rng(42); // Sta³y seed
    for (int y = 0; y < 8; ++y)
        for (int x = 0; x < 8; ++x)
            for (int p = 0; p < 13; ++p)
                zobrist_table[y][x][p] = rng();
    initialized = true;
}

int piece_index(uint8_t piece) {
    PieceType type = getPieceType(piece);
    PieceColor color = getPieceColor(piece);
    if (type == NONE) return 0;
    int base = 0;
    switch (type) {
    case PAWN: base = 1; break;
    case KNIGHT: base = 2; break;
    case BISHOP: base = 3; break;
    case ROOK: base = 4; break;
    case QUEEN: base = 5; break;
    case KING: base = 6; break;
    default: base = 0;
    }
    if (color == WHITE) return base;
    if (color == BLACK) return base + 6;
    return 0;
}

uint64_t zobrist_hash(const Board& board) {
    uint64_t h = 0;
    for (int y = 0; y < 8; ++y)
        for (int x = 0; x < 8; ++x) {
            int idx = piece_index(board.squares[y][x]);
            if (idx != 0)
                h ^= zobrist_table[y][x][idx];
        }
    return h;
}

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

int positional_bonus(PieceType pt, PieceColor color, int y, int x) {
    switch (pt) {
    case PAWN:   return (color == WHITE) ? pawn_table[y][x] : pawn_table[7 - y][x];
    case KNIGHT: return (color == WHITE) ? knight_table[y][x] : knight_table[7 - y][x];
    case BISHOP: return (color == WHITE) ? bishop_table[y][x] : bishop_table[7 - y][x];
    case ROOK:   return (color == WHITE) ? rook_table[y][x] : rook_table[7 - y][x];
    case QUEEN:  return (color == WHITE) ? queen_table[y][x] : queen_table[7 - y][x];
    case KING:   return (color == WHITE) ? king_table[y][x] : king_table[7 - y][x];
    default:     return 0;
    }
}

// --- Null move penalty ---
bool is_null_move(const std::vector<Move>& history, const Move& current) {
    if (history.empty()) return false;
    const Move& prev = history.back();
    return (current.fromY == prev.toY && current.fromX == prev.toX &&
        current.toY == prev.fromY && current.toX == prev.fromX &&
        current.promoPiece == prev.promoPiece);
}

// --- Uniwersalna ocena planszy ---
int evaluate_board(const Board& board, PieceColor color, const std::vector<Move>& moveHistory, int repetitionPenalty) {
    int score = 0;
    int pawnColWhite[8] = { 0 }, pawnColBlack[8] = { 0 };
    int kingCenterPenaltyW = 0, kingCenterPenaltyB = 0;
    int centerControlW = 0, centerControlB = 0;
    int pawnAdvancementW = 0, pawnAdvancementB = 0;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            uint8_t piece = board.squares[y][x];
            if (piece == NONE) continue;
            PieceType pt = getPieceType(piece);
            PieceColor pc = getPieceColor(piece);

            int value = piece_value(pt) + positional_bonus(pt, pc, y, x);
            if (pc == WHITE) score += value;
            else score -= value;

            // Wie¿a na otwartej/pó³otwartej linii – bonus w ocenie
            if (pt == ROOK) {
                bool open = true, semiOpen = true;
                for (int yy = 0; yy < 8; ++yy) {
                    if (getPieceType(board.squares[yy][x]) == PAWN) {
                        if (pc == WHITE && getPieceColor(board.squares[yy][x]) == WHITE) semiOpen = false;
                        if (pc == BLACK && getPieceColor(board.squares[yy][x]) == BLACK) semiOpen = false;
                        open = false;
                    }
                }
                if (pc == WHITE) score += open ? 40 : (semiOpen ? 20 : 0);
                else score -= open ? 40 : (semiOpen ? 20 : 0);
            }

            // Pionki na kolumnach (do izolacji/duplikacji)
            if (pt == PAWN) {
                if (pc == WHITE) pawnColWhite[x]++;
                else pawnColBlack[x]++;
                if (pc == WHITE && y >= 4 && y <= 6) pawnAdvancementW++;
                if (pc == BLACK && y >= 1 && y <= 3) pawnAdvancementB++;
            }

            // Król w centrum
            if (pt == KING) {
                if (pc == WHITE && ((y == 0 && (x == 4 || x == 3)))) kingCenterPenaltyW += 40;
                if (pc == BLACK && ((y == 7 && (x == 4 || x == 3)))) kingCenterPenaltyB += 40;
            }

            // Kontrola centrum
            if ((y == 3 || y == 4) && (x == 3 || x == 4)) {
                if (pc == WHITE) centerControlW++;
                else centerControlB++;
            }
        }
    }

    // Pionki zduplikowane/izolowane
    for (int x = 0; x < 8; ++x) {
        if (pawnColWhite[x] > 1) score -= 10 * (pawnColWhite[x] - 1);
        if (pawnColBlack[x] > 1) score += 10 * (pawnColBlack[x] - 1);
        if (pawnColWhite[x] > 0) {
            bool isolated = true;
            if ((x > 0 && pawnColWhite[x - 1] > 0) || (x < 7 && pawnColWhite[x + 1] > 0)) isolated = false;
            if (isolated) score -= 20;
        }
        if (pawnColBlack[x] > 0) {
            bool isolated = true;
            if ((x > 0 && pawnColBlack[x - 1] > 0) || (x < 7 && pawnColBlack[x + 1] > 0)) isolated = false;
            if (isolated) score += 20;
        }
    }
    score += 15 * pawnAdvancementW;
    score -= 15 * pawnAdvancementB;
    score += 20 * centerControlW;
    score -= 20 * centerControlB;
    score -= kingCenterPenaltyW;
    score += kingCenterPenaltyB;

    // Kara za powtarzanie pozycji
    uint64_t hash = zobrist_hash(board);
    if (positionCounts[hash] > 1) score += (color == WHITE) ? -repetitionPenalty : repetitionPenalty;

    // Null move penalty
    if (!moveHistory.empty() && is_null_move(moveHistory, moveHistory.back()))
        score += (color == WHITE) ? -100 : 100;

    return score;
}

// --- Szybka ocena ruchu (bicie, promocja, bonusy za szach/mat tylko raz) ---
int quick_move_score(const Board& board, const Move& m, PieceColor color) {
    int score = 0;
    uint8_t target = board.squares[m.toY][m.toX];
    if (target != NONE) {
        PieceType ttype = getPieceType(target);
        score += piece_value(ttype) * 100;
        PieceType myType = getPieceType(board.squares[m.fromY][m.fromX]);
        score -= piece_value(myType);
    }
    if (getPieceType(board.squares[m.fromY][m.fromX]) == PAWN &&
        (m.toY == 0 || m.toY == 7) && m.promoPiece != 0) {
        score += 9000;
    }
    return score;
}


std::vector<ScoredMove> score_and_sort_moves(Board& board, PieceColor color, PieceColor enemy) {
    auto moves = board.generateAllMoves(color);
    std::vector<ScoredMove> scoredMoves;
    for (const auto& m : moves) {
        int score = quick_move_score(board, m, color);
        board.makeMove(m.fromY, m.fromX, m.toY, m.toX, m.promoPiece);
        if (board.isCheck(enemy)) {
            if (board.isCheckmate(enemy))
                score += 100000;
            else
                score += 700;
        }
        board.undoMove();
        scoredMoves.push_back({ m, score });
    }
    std::shuffle(scoredMoves.begin(), scoredMoves.end(), rng_engine);
    std::sort(scoredMoves.begin(), scoredMoves.end(), [](const ScoredMove& a, const ScoredMove& b) {
        return a.score > b.score;
        });
    return scoredMoves;
}

// --- MINIMAX Z REFAKTOREM ---
int minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, PieceColor color, std::vector<Move>& moveHistory) {
    uint64_t key = zobrist_hash(board);
    positionCounts[key]++;
    auto it = transTable.find(key);
    if (it != transTable.end() && it->second.depth >= depth) {
        positionCounts[key]--;
        return it->second.value;
    }

    PieceColor enemy = (color == WHITE) ? BLACK : WHITE;

    if (depth == 0 || board.isCheckmate(color) || board.isStalemate(color)) {
        int val = evaluate_board(board, color, moveHistory);
        transTable[key] = { depth, val };
        positionCounts[key]--;
        return val;
    }

    auto scoredMoves = score_and_sort_moves(board, color, enemy);

    if (scoredMoves.empty()) {
        int val = evaluate_board(board, color, moveHistory);
        transTable[key] = { depth, val };
        positionCounts[key]--;
        return val;
    }

    int bestVal;
    if (maximizingPlayer) {
        bestVal = -1000000;
        for (const auto& sm : scoredMoves) {
            const Move& m = sm.move;
            board.makeMove(m.fromY, m.fromX, m.toY, m.toX, m.promoPiece);
            moveHistory.push_back(m);
            int eval = minimax(board, depth - 1, alpha, beta, false, enemy, moveHistory);
            moveHistory.pop_back();
            board.undoMove();
            bestVal = std::max(bestVal, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha)
                break;
        }
    }
    else {
        bestVal = 1000000;
        for (const auto& sm : scoredMoves) {
            const Move& m = sm.move;
            board.makeMove(m.fromY, m.fromX, m.toY, m.toX, m.promoPiece);
            moveHistory.push_back(m);
            int eval = minimax(board, depth - 1, alpha, beta, true, enemy, moveHistory);
            moveHistory.pop_back();
            board.undoMove();
            bestVal = std::min(bestVal, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha)
                break;
        }
    }
    transTable[key] = { depth, bestVal };
    positionCounts[key]--;
    return bestVal;
}

// --- NAJLEPSZY RUCH AI ---
Move get_best_move_AI(Board& board, int depth, PieceColor color) {
    positionCounts.clear();
    PieceColor enemy = (color == WHITE) ? BLACK : WHITE;
    auto scoredMoves = score_and_sort_moves(board, color, enemy);

    Move bestMove;
    int bestEval = (color == WHITE) ? -1000000 : 1000000;
    std::vector<Move> moveHistory;

    for (const auto& sm : scoredMoves) {
        const Move& m = sm.move;
        board.makeMove(m.fromY, m.fromX, m.toY, m.toX, m.promoPiece);
        moveHistory.push_back(m);
        int eval = minimax(board, depth - 1, -1000000, 1000000, color == BLACK, enemy, moveHistory);
        moveHistory.pop_back();
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
