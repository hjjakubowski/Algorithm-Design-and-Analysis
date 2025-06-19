#include "board.hpp"
#include "bitboard_tables.hpp"
#include "piece.hpp"
#include <iostream>
#include <cstring>

Board::Board() { setInitial(); }

void Board::setInitial() {
    memset(pieces, 0, sizeof(pieces));
    memset(occupied, 0, sizeof(occupied));
    all = 0;
    halfmoveClock = 0;


    pieces[WHITE][ROOK] |= (1ULL << sq(0, 0)) | (1ULL << sq(0, 7));
    pieces[WHITE][KNIGHT] |= (1ULL << sq(0, 1)) | (1ULL << sq(0, 6));
    pieces[WHITE][BISHOP] |= (1ULL << sq(0, 2)) | (1ULL << sq(0, 5));
    pieces[WHITE][QUEEN] |= (1ULL << sq(0, 3));
    pieces[WHITE][KING] |= (1ULL << sq(0, 4));
    for (int x = 0; x < 8; ++x) pieces[WHITE][PAWN] |= (1ULL << sq(1, x));

    pieces[BLACK][ROOK] |= (1ULL << sq(7, 0)) | (1ULL << sq(7, 7));
    pieces[BLACK][KNIGHT] |= (1ULL << sq(7, 1)) | (1ULL << sq(7, 6));
    pieces[BLACK][BISHOP] |= (1ULL << sq(7, 2)) | (1ULL << sq(7, 5));
    pieces[BLACK][QUEEN] |= (1ULL << sq(7, 3));
    pieces[BLACK][KING] |= (1ULL << sq(7, 4));
    for (int x = 0; x < 8; ++x) pieces[BLACK][PAWN] |= (1ULL << sq(6, x));

    for (int c = 0; c < 2; ++c) {
        occupied[c] = 0;
        for (int t = 0; t < 6; ++t) occupied[c] |= pieces[c][t];
        all |= occupied[c];
    }
    history.clear();
}

void Board::print() const {
    std::cout << "  +-----------------+\n";
    for (int y = 7; y >= 0; --y) {
        std::cout << y + 1 << " |";
        for (int x = 0; x < 8; ++x) {
            int sq_idx = sq(y, x);
            char c = '.';
            for (int color = 0; color < 2; ++color)
                for (int pt = 0; pt < 6; ++pt)
                    if (pieces[color][pt] & (1ULL << sq_idx))
                        c = pieceToChar((PieceType)pt, (PieceColor)color);
            std::cout << " " << c;
        }
        std::cout << " |\n";
    }
    std::cout << "  +-----------------+\n";
    std::cout << "    a b c d e f g h\n";
}

inline void push_moves_from_mask(std::vector<Move>& moves, int from, Bitboard mask, const Board& board, PieceColor color, PieceType promo = NONE_TYPE) {
    Bitboard own = board.occupied[color];
    while (mask) {
        int to = bit_scan_forward(mask);
        if (!(own & (1ULL << to)))
            moves.emplace_back(from, to, promo);
        mask &= mask - 1;
    }
}

std::vector<Move> pawnMoves(const Board& board, PieceColor color) {
    std::vector<Move> moves;
    Bitboard pawns = board.pieces[color][PAWN];
    Bitboard empty = ~board.all;
    int dir = (color == WHITE) ? 8 : -8;
    int finalRank = (color == WHITE) ? 7 : 0;

    Bitboard singlePush = (color == WHITE) ? (pawns << 8) : (pawns >> 8);
    singlePush &= empty;
    Bitboard pawnsToMove = singlePush;
    while (pawnsToMove) {
        int to = bit_scan_forward(pawnsToMove);
        int from = to - dir;
        if (rank_of(to) == finalRank) {
            moves.emplace_back(from, to, QUEEN);
            moves.emplace_back(from, to, ROOK);
            moves.emplace_back(from, to, KNIGHT);
            moves.emplace_back(from, to, BISHOP);
        }
        else {
            moves.emplace_back(from, to);
        }
        pawnsToMove &= pawnsToMove - 1;
    }

    Bitboard doublePush;
    if (color == WHITE) {
        doublePush = ((pawns & (0xFFULL << 8)) << 16) & empty & (empty << 8);
    }
    else {
        doublePush = ((pawns & (0xFFULL << 48)) >> 16) & empty & (empty >> 8);
    }
    pawnsToMove = doublePush;
    while (pawnsToMove) {
        int to = bit_scan_forward(pawnsToMove);
        int from = to - 2 * dir;
        moves.emplace_back(from, to);
        pawnsToMove &= pawnsToMove - 1;
    }

    Bitboard enemy = board.occupied[!color];
    Bitboard leftAttack = (color == WHITE) ? (pawns << 7) : (pawns >> 9);
    Bitboard rightAttack = (color == WHITE) ? (pawns << 9) : (pawns >> 7);

    leftAttack &= enemy & ~0x0101010101010101ULL; 
    rightAttack &= enemy & ~0x8080808080808080ULL;

    pawnsToMove = leftAttack;
    while (pawnsToMove) {
        int to = bit_scan_forward(pawnsToMove);
        int from = to - ((color == WHITE) ? 7 : -9);
        if (rank_of(to) == finalRank) {
            moves.emplace_back(from, to, QUEEN);
            moves.emplace_back(from, to, ROOK);
            moves.emplace_back(from, to, KNIGHT);
            moves.emplace_back(from, to, BISHOP);
        }
        else {
            moves.emplace_back(from, to);
        }
        pawnsToMove &= pawnsToMove - 1;
    }
    pawnsToMove = rightAttack;
    while (pawnsToMove) {
        int to = bit_scan_forward(pawnsToMove);
        int from = to - ((color == WHITE) ? 9 : -7);
        if (rank_of(to) == finalRank) {
            moves.emplace_back(from, to, QUEEN);
            moves.emplace_back(from, to, ROOK);
            moves.emplace_back(from, to, KNIGHT);
            moves.emplace_back(from, to, BISHOP);
        }
        else {
            moves.emplace_back(from, to);
        }
        pawnsToMove &= pawnsToMove - 1;
    }
    return moves;
}


std::vector<Move> knightMoves(const Board& board, PieceColor color) {
    std::vector<Move> moves;
    Bitboard knights = board.pieces[color][KNIGHT];
    while (knights) {
        int from = bit_scan_forward(knights);
        Bitboard mask = knight_attacks[from] & ~board.occupied[color];
        push_moves_from_mask(moves, from, mask, board, color);
        knights &= knights - 1;
    }
    return moves;
}

std::vector<Move> kingMoves(const Board& board, PieceColor color) {
    std::vector<Move> moves;
    Bitboard king = board.pieces[color][KING];
    if (!king) return moves;
    int from = bit_scan_forward(king);
    Bitboard mask = king_attacks[from] & ~board.occupied[color];
    push_moves_from_mask(moves, from, mask, board, color);
    return moves;
}

Bitboard bishop_attacks(int from, const Board& board) {
    Bitboard result = 0;
    int y = from / 8, x = from % 8;
    for (int dy = -1; dy <= 1; dy += 2) for (int dx = -1; dx <= 1; dx += 2) {
        int ny = y + dy, nx = x + dx;
        while (ny >= 0 && ny < 8 && nx >= 0 && nx < 8) {
            int to = ny * 8 + nx;
            result |= (1ULL << to);
            if (board.all & (1ULL << to)) break;
            ny += dy; nx += dx;
        }
    }
    return result;
}
Bitboard rook_attacks(int from, const Board& board) {
    Bitboard result = 0;
    int y = from / 8, x = from % 8;
    for (int d = -1; d <= 1; d += 2) {
        int ny = y + d;
        while (ny >= 0 && ny < 8) {
            int to = ny * 8 + x;
            result |= (1ULL << to);
            if (board.all & (1ULL << to)) break;
            ny += d;
        }
        int nx = x + d;
        while (nx >= 0 && nx < 8) {
            int to = y * 8 + nx;
            result |= (1ULL << to);
            if (board.all & (1ULL << to)) break;
            nx += d;
        }
    }
    return result;
}

std::vector<Move> bishopMoves(const Board& board, PieceColor color) {
    std::vector<Move> moves;
    Bitboard bishops = board.pieces[color][BISHOP];
    while (bishops) {
        int from = bit_scan_forward(bishops);
        Bitboard mask = bishop_attacks(from, board) & ~board.occupied[color];
        push_moves_from_mask(moves, from, mask, board, color);
        bishops &= bishops - 1;
    }
    return moves;
}
std::vector<Move> rookMoves(const Board& board, PieceColor color) {
    std::vector<Move> moves;
    Bitboard rooks = board.pieces[color][ROOK];
    while (rooks) {
        int from = bit_scan_forward(rooks);
        Bitboard mask = rook_attacks(from, board) & ~board.occupied[color];
        push_moves_from_mask(moves, from, mask, board, color);
        rooks &= rooks - 1;
    }
    return moves;
}
std::vector<Move> queenMoves(const Board& board, PieceColor color) {
    std::vector<Move> moves;
    Bitboard queens = board.pieces[color][QUEEN];
    while (queens) {
        int from = bit_scan_forward(queens);
        Bitboard mask = (rook_attacks(from, board) | bishop_attacks(from, board)) & ~board.occupied[color];
        push_moves_from_mask(moves, from, mask, board, color);
        queens &= queens - 1;
    }
    return moves;
}

std::vector<Move> Board::generateAllMoves(PieceColor color) const {
    std::vector<Move> moves;
    auto pawns = pawnMoves(*this, color); moves.insert(moves.end(), pawns.begin(), pawns.end());
    auto knights = knightMoves(*this, color); moves.insert(moves.end(), knights.begin(), knights.end());
    auto bishops = bishopMoves(*this, color); moves.insert(moves.end(), bishops.begin(), bishops.end());
    auto rooks = rookMoves(*this, color); moves.insert(moves.end(), rooks.begin(), rooks.end());
    auto queens = queenMoves(*this, color); moves.insert(moves.end(), queens.begin(), queens.end());
    auto kings = kingMoves(*this, color); moves.insert(moves.end(), kings.begin(), kings.end());
    return moves;
}

bool Board::makeMove(const Move& m, PieceColor color) {
    HistoryEntry h;
    memcpy(h.pieces, pieces, sizeof(pieces));
    memcpy(h.occupied, occupied, sizeof(occupied));
    h.all = all;
    h.move = m;
    h.halfmoveClock = halfmoveClock; 
    history.push_back(h);

    bool isPawnMove = false;
    if (pieces[color][PAWN] & (1ULL << m.from)) isPawnMove = true;
    bool isCapture = false;
    for (int t = 0; t < 6; ++t) {
        for (int c = 0; c < 2; ++c) {
            if (pieces[c][t] & (1ULL << m.to)) {
                isCapture = true;
                break;
            }
        }
    }

    for (int t = 0; t < 6; ++t)
        for (int c = 0; c < 2; ++c)
            pieces[c][t] &= ~(1ULL << m.to);

    if (m.promoPiece != NONE_TYPE && pieces[color][PAWN] & (1ULL << m.from)) {
        pieces[color][PAWN] &= ~(1ULL << m.from);
        pieces[color][m.promoPiece] |= (1ULL << m.to);
    }
    else {
        for (int t = 0; t < 6; ++t) {
            if (pieces[color][t] & (1ULL << m.from)) {
                pieces[color][t] &= ~(1ULL << m.from);
                pieces[color][t] |= (1ULL << m.to);
                break;
            }
        }
    }

    for (int c = 0; c < 2; ++c) {
        occupied[c] = 0;
        for (int t = 0; t < 6; ++t)
            occupied[c] |= pieces[c][t];
    }
    all = occupied[WHITE] | occupied[BLACK];

    if (isPawnMove || isCapture)
        halfmoveClock = 0;
    else
        halfmoveClock++;

    return true;
}

void Board::undoMove() {
    if (history.empty()) return;
    const auto& h = history.back();
    memcpy(pieces, h.pieces, sizeof(pieces));
    memcpy(occupied, h.occupied, sizeof(occupied));
    all = h.all;
    halfmoveClock = h.halfmoveClock; 
    history.pop_back();
}

bool Board::isLegalMove(const Move& m, PieceColor color) const {
    auto moves = generateAllMoves(color);
    for (const auto& move : moves)
        if (move.from == m.from && move.to == m.to && move.promoPiece == m.promoPiece)
            return true;
    return false;
}

bool Board::isCheck(PieceColor color) const {
    int kingSq = findKing(color);
    PieceColor enemy = (color == WHITE) ? BLACK : WHITE;
    auto moves = generateAllMoves(enemy);
    for (const auto& m : moves)
        if (m.to == kingSq)
            return true;
    return false;
}

bool Board::isCheckmate(PieceColor color) const {
    if (!isCheck(color)) return false;
    auto moves = generateAllMoves(color);
    Board tmp = *this;
    for (const auto& m : moves) {
        tmp = *this;
        tmp.makeMove(m, color);
        if (!tmp.isCheck(color)) return false;
    }
    return true;
}

bool Board::isStalemate(PieceColor color) const {
    if (isCheck(color)) return false;
    auto moves = generateAllMoves(color);
    Board tmp = *this;
    for (const auto& m : moves) {
        tmp = *this;
        tmp.makeMove(m, color);
        if (!tmp.isCheck(color)) return false;
    }
    return moves.empty();
}

int Board::findKing(PieceColor color) const {
    Bitboard bb = pieces[color][KING];
    if (bb) return bit_scan_forward(bb);
    return -1;
}
