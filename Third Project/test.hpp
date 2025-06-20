#pragma once
#include <cassert>
#include <string>
#include <iostream>
#include "board.hpp"
#include "move.hpp" 

// Zwraca true, jezli na danym polu jest figura o danym znaku ('P', 'p', 'R', 'k' itd.)
bool expect_piece(const Board& board, int y, int x, char c) {
    return pieceToChar(board.squares[y][x]) == c;
}

void test_chess_engine() {
    Board board;
    // 1. Test poczatkowego ustawienia
    assert(expect_piece(board, 0, 0, 'R'));
    assert(expect_piece(board, 1, 0, 'P'));
    assert(expect_piece(board, 7, 4, 'k'));
    assert(expect_piece(board, 6, 7, 'p'));

    // 2. Test prostego ruchu pionka
    assert(board.isLegalMove(1, 4, 3, 4)); // e2e4
    board.makeMove(1, 4, 3, 4);
    assert(expect_piece(board, 3, 4, 'P'));
    assert(expect_piece(board, 1, 4, '.'));

    // 3. Test cofania ruchu
    board.undoMove();
    assert(expect_piece(board, 1, 4, 'P'));
    assert(expect_piece(board, 3, 4, '.'));


    // 4. Test bicia pionkiem
    board.squares[2][3] = makePiece(PAWN, BLACK); // ustaw czarnego pionka na d3
    assert(board.isLegalMove(1, 4, 2, 3)); // e2d3
    board.makeMove(1, 4, 2, 3);
    assert(expect_piece(board, 2, 3, 'P'));
    assert(expect_piece(board, 3, 3, '.'));

    // 5. Test promocji pionka
    board.squares[7][0] = NONE; // Upewnij sie, ze pole promocji jest puste!
    board.squares[6][0] = makePiece(PAWN, WHITE); // biale na a7
    assert(board.isLegalMove(6, 0, 7, 0, QUEEN));
    board.makeMove(6, 0, 7, 0, QUEEN);
    assert(expect_piece(board, 7, 0, 'Q'));


    // 6. Test bicie skoczkiem i cofanie
    board.setInitial();
    board.makeMove(0, 1, 2, 2); // b1c3
    board.makeMove(7, 1, 5, 2); // b8c6
    board.makeMove(2, 2, 4, 3); // c3d5
    board.makeMove(5, 2, 4, 3); // c6d5 (bije bialego skoczka)
    assert(expect_piece(board, 4, 3, 'n'));
    board.undoMove();
    assert(expect_piece(board, 4, 3, 'N'));

    // 7. Test mata: prosty mat dwu-ruchowy
    board.setInitial();
    board.makeMove(1, 5, 2, 5); // f2f3
    board.makeMove(6, 4, 4, 4); // e7e5
    board.makeMove(1, 6, 3, 6); // g2g4
    board.makeMove(7, 3, 3, 7); // Hetman h4# (d8h4)
    assert(board.isCheckmate(WHITE));

    // 8. Test pata: sam krol vs krol
    board.setInitial();
    for (int y = 0; y < 8; ++y) for (int x = 0; x < 8; ++x) board.squares[y][x] = NONE;
    board.squares[0][0] = makePiece(KING, WHITE);
    board.squares[7][7] = makePiece(KING, BLACK);
    assert(board.isStalemate(WHITE) == false);
    assert(board.isCheckmate(WHITE) == false);

    std::cout << "ALL TESTS PASSED!\n";
}