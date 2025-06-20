#include "board.hpp"
#include <iostream>
#include <cassert>

Board::Board() { setInitial(); }

void Board::setInitial() {
    squares[0] = { makePiece(ROOK, WHITE), makePiece(KNIGHT, WHITE), makePiece(BISHOP, WHITE), makePiece(QUEEN, WHITE),
                   makePiece(KING, WHITE), makePiece(BISHOP, WHITE), makePiece(KNIGHT, WHITE), makePiece(ROOK, WHITE) };
    squares[1].fill(makePiece(PAWN, WHITE));
    for (int y = 2; y < 6; ++y)
        squares[y].fill(makePiece(NONE, EMPTY));
    squares[6].fill(makePiece(PAWN, BLACK));
    squares[7] = { makePiece(ROOK, BLACK), makePiece(KNIGHT, BLACK), makePiece(BISHOP, BLACK), makePiece(QUEEN, BLACK),
                   makePiece(KING, BLACK), makePiece(BISHOP, BLACK), makePiece(KNIGHT, BLACK), makePiece(ROOK, BLACK) };
    history.clear();
    halfmoveClock = 0;
}

void Board::print() const {
    std::cout << "  +-----------------+\n";
    for (int y = 7; y >= 0; --y) {
        std::cout << y + 1 << " |";
        for (int x = 0; x < 8; ++x) {
            std::cout << " " << pieceToChar(squares[y][x]);
        }
        std::cout << " |\n";
    }
    std::cout << "  +-----------------+\n";
    std::cout << "    a b c d e f g h\n";
}

bool Board::isLegalMove(int fromY, int fromX, int toY, int toX, uint8_t promo) const {
    PieceColor color = getPieceColor(squares[fromY][fromX]);
    if (color == EMPTY) return false;
    std::vector<Move> moves;
    PieceType type = getPieceType(squares[fromY][fromX]);
    switch (type) {
    case PAWN:   moves = legalPawnMoves(fromY, fromX); break;
    case KNIGHT: moves = legalKnightMoves(fromY, fromX); break;
    case BISHOP: moves = legalBishopMoves(fromY, fromX); break;
    case ROOK:   moves = legalRookMoves(fromY, fromX); break;
    case QUEEN:  moves = legalQueenMoves(fromY, fromX); break;
    case KING:   moves = legalKingMoves(fromY, fromX); break;
    default:     return false;
    }

    for (const auto& m : moves) {
        if (m.toY == toY && m.toX == toX && m.fromY == fromY && m.fromX == fromX &&
            (type != PAWN || m.promoPiece == promo))
            return true;
    }

    return false;
}

bool Board::makeMove(int fromY, int fromX, int toY, int toX, uint8_t promo) {
    uint8_t piece = squares[fromY][fromX];
    uint8_t captured = squares[toY][toX];
    history.push_back({ squares, Move(fromY, fromX, toY, toX, promo), halfmoveClock });

    // Regu³a 50 ruchów: jeœli ruch pionkiem lub bicie, resetuj licznik, w przeciwnym razie inkrementuj
    if (getPieceType(piece) == PAWN || captured != NONE) {
        halfmoveClock = 0;
    }
    else {
        halfmoveClock++;
    }

    squares[toY][toX] = piece;
    squares[fromY][fromX] = NONE;

    if (getPieceType(piece) == PAWN && (toY == 0 || toY == 7)) {
        squares[toY][toX] = makePiece(static_cast<PieceType>(promo), getPieceColor(piece));
    }
    return true;
}

void Board::undoMove() {
    if (history.empty()) return;
    squares = history.back().state;
    halfmoveClock = history.back().halfmoveClock;
    history.pop_back();
}

std::pair<int, int> Board::findKing(PieceColor color) const {
    for (int y = 0; y < 8; ++y)
        for (int x = 0; x < 8; ++x)
            if (getPieceType(squares[y][x]) == KING && getPieceColor(squares[y][x]) == color)
                return { y, x };
    return { -1, -1 };
}

bool Board::isCheck(PieceColor color) const {
    auto [ky, kx] = findKing(color);
    if (ky == -1) return false;
    PieceColor enemy = (color == WHITE ? BLACK : WHITE);
    auto moves = generateAllMoves(enemy);
    for (const auto& m : moves)
        if (m.toY == ky && m.toX == kx) return true;
    return false;
}

bool Board::isCheckmate(PieceColor color) const {
    if (!isCheck(color)) return false;
    auto moves = generateAllMoves(color);
    for (const auto& m : moves) {
        const_cast<Board*>(this)->makeMove(m.fromY, m.fromX, m.toY, m.toX, m.promoPiece);
        bool notCheck = !isCheck(color);
        const_cast<Board*>(this)->undoMove();
        if (notCheck) return false;
    }
    return true;
}

bool Board::isStalemate(PieceColor color) const {
    if (isCheck(color)) return false;
    auto moves = generateAllMoves(color);
    for (const auto& m : moves) {
        const_cast<Board*>(this)->makeMove(m.fromY, m.fromX, m.toY, m.toX, m.promoPiece);
        bool notCheck = !isCheck(color);
        const_cast<Board*>(this)->undoMove();
        if (notCheck) return false;
    }
    return true;
}

// --- Nowe regu³y remisu ---

bool Board::isDrawBy50MoveRule() const {
    return halfmoveClock >= 100;
}

bool Board::isInsufficientMaterial() const {
    int whiteBishops = 0, blackBishops = 0, whiteKnights = 0, blackKnights = 0;
    int whiteOther = 0, blackOther = 0;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            uint8_t piece = squares[y][x];
            PieceType type = getPieceType(piece);
            PieceColor color = getPieceColor(piece);
            if (type == NONE) continue;
            if (type == KING) continue;
            if (type == BISHOP) { (color == WHITE ? whiteBishops : blackBishops)++; continue; }
            if (type == KNIGHT) { (color == WHITE ? whiteKnights : blackKnights)++; continue; }
            if (color == WHITE) whiteOther++;
            else if (color == BLACK) blackOther++;
        }
    }
    // Król vs Król
    if (whiteBishops == 0 && blackBishops == 0 && whiteKnights == 0 && blackKnights == 0 && whiteOther == 0 && blackOther == 0)
        return true;
    // Król + lekka figura vs król
    if ((whiteBishops == 1 && whiteKnights == 0 && whiteOther == 0 && blackBishops == 0 && blackKnights == 0 && blackOther == 0) ||
        (blackBishops == 1 && blackKnights == 0 && blackOther == 0 && whiteBishops == 0 && whiteKnights == 0 && whiteOther == 0) ||
        (whiteKnights == 1 && whiteBishops == 0 && whiteOther == 0 && blackBishops == 0 && blackKnights == 0 && blackOther == 0) ||
        (blackKnights == 1 && blackBishops == 0 && blackOther == 0 && whiteBishops == 0 && whiteKnights == 0 && whiteOther == 0))
        return true;
    // Król + skoczek vs król + skoczek
    if (whiteKnights == 1 && blackKnights == 1 && whiteBishops == 0 && blackBishops == 0 && whiteOther == 0 && blackOther == 0)
        return true;
    return false;
}

// --- Generowanie ruchów ---

std::vector<Move> Board::legalPawnMoves(int y, int x) const {
    std::vector<Move> moves;
    uint8_t piece = squares[y][x];
    PieceColor color = getPieceColor(piece);
    int direction = (color == WHITE) ? 1 : -1;
    int startRow = (color == WHITE) ? 1 : 6;
    int promoteRow = (color == WHITE) ? 7 : 0;
    int ny = y + direction;

    // Proste ruchy do przodu
    if (ny >= 0 && ny < 8) {
        if (squares[ny][x] == NONE) {
            if (ny == promoteRow) {
                moves.emplace_back(y, x, ny, x, QUEEN);
                moves.emplace_back(y, x, ny, x, ROOK);
                moves.emplace_back(y, x, ny, x, KNIGHT);
                moves.emplace_back(y, x, ny, x, BISHOP);
            }
            else {
                moves.emplace_back(y, x, ny, x);
            }
            // Podwójny ruch z pierwszego pola 
            if (y == startRow && squares[y + direction][x] == NONE && squares[y + 2 * direction][x] == NONE) {
                moves.emplace_back(y, x, y + 2 * direction, x);
            }
        }
        // Bicia
        for (int dx = -1; dx <= 1; dx += 2) {
            int nx = x + dx;
            if (nx >= 0 && nx < 8) {
                uint8_t target = squares[ny][nx];
                if (target != NONE && getPieceColor(target) != color) {
                    if (ny == promoteRow) {
                        moves.emplace_back(y, x, ny, nx, QUEEN);
                        moves.emplace_back(y, x, ny, nx, ROOK);
                        moves.emplace_back(y, x, ny, nx, KNIGHT);
                        moves.emplace_back(y, x, ny, nx, BISHOP);
                    }
                    else {
                        moves.emplace_back(y, x, ny, nx);
                    }
                }
            }
        }
    }
    return moves;
}

std::vector<Move> Board::legalKnightMoves(int y, int x) const {
    std::vector<Move> moves;
    static const int dy[8] = { -2, -1, 1, 2, 2, 1, -1, -2 };
    static const int dx[8] = { 1, 2, 2, 1, -1, -2, -2, -1 };
    PieceColor color = getPieceColor(squares[y][x]);
    for (int d = 0; d < 8; ++d) {
        int ny = y + dy[d], nx = x + dx[d];
        if (ny >= 0 && ny < 8 && nx >= 0 && nx < 8) {
            uint8_t target = squares[ny][nx];
            if (target == NONE || getPieceColor(target) != color) {
                moves.emplace_back(y, x, ny, nx);
            }
        }
    }
    return moves;
}

std::vector<Move> Board::legalBishopMoves(int y, int x) const {
    std::vector<Move> moves;
    PieceColor color = getPieceColor(squares[y][x]);
    static const int dirs[4][2] = { {1,1},{1,-1},{-1,1},{-1,-1} };
    for (auto& d : dirs) {
        int ny = y, nx = x;
        while (true) {
            ny += d[0]; nx += d[1];
            if (ny < 0 || ny >= 8 || nx < 0 || nx >= 8) break;
            uint8_t target = squares[ny][nx];
            if (target == NONE) {
                moves.emplace_back(y, x, ny, nx);
            }
            else {
                if (getPieceColor(target) != color)
                    moves.emplace_back(y, x, ny, nx);
                break;
            }
        }
    }
    return moves;
}

std::vector<Move> Board::legalRookMoves(int y, int x) const {
    std::vector<Move> moves;
    PieceColor color = getPieceColor(squares[y][x]);
    static const int dirs[4][2] = { {1,0},{-1,0},{0,1},{0,-1} };
    for (auto& d : dirs) {
        int ny = y, nx = x;
        while (true) {
            ny += d[0]; nx += d[1];
            if (ny < 0 || ny >= 8 || nx < 0 || nx >= 8) break;
            uint8_t target = squares[ny][nx];
            if (target == NONE) {
                moves.emplace_back(y, x, ny, nx);
            }
            else {
                if (getPieceColor(target) != color)
                    moves.emplace_back(y, x, ny, nx);
                break;
            }
        }
    }
    return moves;
}

std::vector<Move> Board::legalQueenMoves(int y, int x) const {
    std::vector<Move> moves;
    auto b = legalBishopMoves(y, x);
    auto r = legalRookMoves(y, x);
    moves.insert(moves.end(), b.begin(), b.end());
    moves.insert(moves.end(), r.begin(), r.end());
    return moves;
}

std::vector<Move> Board::legalKingMoves(int y, int x) const {
    std::vector<Move> moves;
    PieceColor color = getPieceColor(squares[y][x]);
    static const int dirs[8][2] = {
        {1,0},{-1,0},{0,1},{0,-1},
        {1,1},{1,-1},{-1,1},{-1,-1}
    };
    for (auto& d : dirs) {
        int ny = y + d[0], nx = x + d[1];
        if (ny >= 0 && ny < 8 && nx >= 0 && nx < 8) {
            uint8_t target = squares[ny][nx];
            if (target == NONE || getPieceColor(target) != color)
                moves.emplace_back(y, x, ny, nx);
        }
    }
    return moves;
}

std::vector<Move> Board::generateAllMoves(PieceColor color) const {
    std::vector<Move> moves;
    for (int y = 0; y < 8; ++y)
        for (int x = 0; x < 8; ++x)
            if (getPieceColor(squares[y][x]) == color) {
                PieceType type = getPieceType(squares[y][x]);
                std::vector<Move> figMoves;
                switch (type) {
                case PAWN:   figMoves = legalPawnMoves(y, x); break;
                case KNIGHT: figMoves = legalKnightMoves(y, x); break;
                case BISHOP: figMoves = legalBishopMoves(y, x); break;
                case ROOK:   figMoves = legalRookMoves(y, x); break;
                case QUEEN:  figMoves = legalQueenMoves(y, x); break;
                case KING:   figMoves = legalKingMoves(y, x); break;
                default: break;
                }
                moves.insert(moves.end(), figMoves.begin(), figMoves.end());
            }
    return moves;
}
