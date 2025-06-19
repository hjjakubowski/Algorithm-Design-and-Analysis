#include "board.hpp"
#include "move.hpp"
#include <cassert>
#include <iostream>
#include <vector>

bool expect_piece(const Board& board, int y, int x, PieceType expectedType, PieceColor expectedColor) {
    int sq_idx = sq(y, x);
    for (int color = 0; color < 2; ++color) {
        for (int type = 0; type < 6; ++type) {
            if (board.pieces[color][type] & (1ULL << sq_idx)) {
                std::cout << "On (" << y << "," << x << ") found type=" << type << " color=" << color << std::endl;
                return (type == expectedType && color == expectedColor);
            }
        }
    }
    return (expectedType == NONE_TYPE && expectedColor == NO_COLOR);
}

void test_chess_engine() {
    Board board;
    board.setInitial();
    // 1. Test pocz¹tkowego ustawienia
    assert(expect_piece(board, 0, 0, ROOK, WHITE));
    assert(expect_piece(board, 1, 0, PAWN, WHITE));
    assert(expect_piece(board, 7, 4, KING, BLACK));
    assert(expect_piece(board, 6, 7, PAWN, BLACK));

    // 2. Test prostego ruchu pionka
    Move e2e4 = Move::fromString("e2e4");
    assert(board.isLegalMove(e2e4, WHITE));
    board.makeMove(e2e4, WHITE);
    assert(expect_piece(board, 3, 4, PAWN, WHITE));
    assert(expect_piece(board, 1, 4, NONE_TYPE, NO_COLOR));

    // 3. Test cofania ruchu
    board.undoMove();
    assert(expect_piece(board, 1, 4, PAWN, WHITE));
    assert(expect_piece(board, 3, 4, NONE_TYPE, NO_COLOR));

    // 4. Test bicia pionkiem
    // Ustaw czarnego pionka na d3
    board.pieces[BLACK][PAWN] |= (1ULL << sq(2, 3));
    board.occupied[BLACK] |= (1ULL << sq(2, 3));
    board.all |= (1ULL << sq(2, 3));
    Move e2d3 = Move::fromString("e2d3");
    assert(board.isLegalMove(e2d3, WHITE));
    board.makeMove(e2d3, WHITE);
    assert(expect_piece(board, 2, 3, PAWN, WHITE));
    assert(expect_piece(board, 3, 3, NONE_TYPE, NO_COLOR));

    // 5. Test promocji pionka
    board.setInitial();
    // Wyczyœæ a8, ustaw bia³ego pionka na a7
    // Wyczyœæ a8 i a7 dla wszystkich figur i kolorów
    for (int t = 0; t < 6; ++t) {
        board.pieces[WHITE][t] &= ~(1ULL << sq(7, 0));
        board.pieces[BLACK][t] &= ~(1ULL << sq(7, 0));
        board.pieces[WHITE][t] &= ~(1ULL << sq(6, 0));
        board.pieces[BLACK][t] &= ~(1ULL << sq(6, 0));
    }
    board.occupied[WHITE] &= ~((1ULL << sq(7, 0)) | (1ULL << sq(6, 0)));
    board.occupied[BLACK] &= ~((1ULL << sq(7, 0)) | (1ULL << sq(6, 0)));
    board.all &= ~((1ULL << sq(7, 0)) | (1ULL << sq(6, 0)));

    // Ustaw bia³ego pionka na a7
    board.pieces[WHITE][PAWN] |= (1ULL << sq(6, 0));
    board.occupied[WHITE] |= (1ULL << sq(6, 0));
    board.all |= (1ULL << sq(6, 0));


    Move a7a8Q(sq(6, 0), sq(7, 0), QUEEN);
    assert(board.isLegalMove(a7a8Q, WHITE));
    board.makeMove(a7a8Q, WHITE);
    assert(expect_piece(board, 7, 0, QUEEN, WHITE));
    std::cout << "AFTER PROMOTION: " << std::endl;
    std::cout << "PAWN: " << ((board.pieces[WHITE][PAWN] & (1ULL << sq(6, 0))) ? "YES" : "NO") << std::endl;
    std::cout << "QUEEN: " << ((board.pieces[WHITE][QUEEN] & (1ULL << sq(7, 0))) ? "YES" : "NO") << std::endl;
    std::cout << "OCCUPIED a7: " << ((board.occupied[WHITE] & (1ULL << sq(6, 0))) ? "YES" : "NO") << std::endl;

    assert(expect_piece(board, 6, 0, NONE_TYPE, NO_COLOR));

    // 6. Test bicie skoczkiem i cofanie
    board.setInitial();
    Move b1c3 = Move::fromString("b1c3");
    Move b8c6 = Move::fromString("b8c6");
    Move c3d5 = Move::fromString("c3d5");
    Move c6d5 = Move::fromString("c6d5");
    board.makeMove(b1c3, WHITE);
    board.makeMove(b8c6, BLACK);
    board.makeMove(c3d5, WHITE);
    board.makeMove(c6d5, BLACK); // bije bia³ego skoczka
    assert(expect_piece(board, 4, 3, KNIGHT, BLACK));
    board.undoMove();
    assert(expect_piece(board, 4, 3, KNIGHT, WHITE));

    // 7. Test mata: prosty mat dwu-ruchowy
    board.setInitial();
    Move f2f3 = Move::fromString("f2f3");
    Move e7e5 = Move::fromString("e7e5");
    Move g2g4 = Move::fromString("g2g4");
    Move d8h4 = Move::fromString("d8h4");
    board.makeMove(f2f3, WHITE);
    board.makeMove(e7e5, BLACK);
    board.makeMove(g2g4, WHITE);
    board.makeMove(d8h4, BLACK);
    assert(board.isCheckmate(WHITE));

    // 8. Test pata: sam król vs król
    board.setInitial();
    for (int y = 0; y < 8; ++y)
        for (int x = 0; x < 8; ++x)
            for (int color = 0; color < 2; ++color)
                for (int t = 0; t < 6; ++t)
                    board.pieces[color][t] &= ~(1ULL << sq(y, x));
    board.occupied[WHITE] = 0;
    board.occupied[BLACK] = 0;
    board.all = 0;
    board.pieces[WHITE][KING] |= (1ULL << sq(0, 0));
    board.pieces[BLACK][KING] |= (1ULL << sq(7, 7));
    board.occupied[WHITE] |= (1ULL << sq(0, 0));
    board.occupied[BLACK] |= (1ULL << sq(7, 7));
    board.all |= (1ULL << sq(0, 0));
    board.all |= (1ULL << sq(7, 7));
    assert(board.isStalemate(WHITE) == false);
    assert(board.isCheckmate(WHITE) == false);

    std::cout << "ALL TESTS PASSED!\n";
}
