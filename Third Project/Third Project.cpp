#include "board.hpp"
#include "move.hpp"
#include <iostream>
#include <string>
#include "test.hpp"


int main() {

    test_chess_engine();
   
    Board board;
    PieceColor turn = WHITE;
    board.print();

    while (true) {
        if (board.isCheckmate(turn)) {
            std::cout << "Mat! " << (turn == WHITE ? "Czarne" : "Biale") << " wygrywaja.\n";
            break;
        }
        if (board.isStalemate(turn)) {
            std::cout << "Pat! Remis.\n";
            break;
        }
        if (board.isCheck(turn)) {
            std::cout << "Szach!\n";
        }

        std::cout << ((turn == WHITE) ? "Biale" : "Czarne") << " podaj ruch (np. e2e4 lub undo): ";
        std::string input;
        std::cin >> input;

        if (input == "undo") {
            board.undoMove();
            turn = (turn == WHITE) ? BLACK : WHITE;
            board.print();
            continue;
        }
        
        if (input == "quit") {
			std::cout << "Koniec gry.\n";
			break;
        }

        if (input.length() < 4 || input.length() > 5) {
            std::cout << "Zly format ruchu!\n";
            continue;
        }

        Move move = Move::fromString(input);
        if (move.fromY < 0 || move.fromY > 7 || move.fromX < 0 || move.fromX > 7 ||
            move.toY < 0 || move.toY > 7 || move.toX < 0 || move.toX > 7) {
            std::cout << "Niepoprawne pole!\n";
            continue;
        }
        if (getPieceType(board.squares[move.fromY][move.fromX]) == PAWN) {
            std::cout << "Legalne ruchy pionka z " << char('a' + move.fromX) << (move.fromY + 1) << ": ";
            auto moves = board.legalPawnMoves(move.fromY, move.fromX);
            for (auto& m : moves) {
                std::cout << char('a' + m.toX) << (m.toY + 1) << " ";
            }
            std::cout << std::endl;
        }


        uint8_t srcPiece = board.squares[move.fromY][move.fromX];
        if (getPieceColor(srcPiece) != turn) {
            std::cout << "To nie twoja figura!\n";
            continue;
        }

        if (!board.isLegalMove(move.fromY, move.fromX, move.toY, move.toX, move.promoPiece)) {
            std::cout << "Nielegalny ruch!\n";
            continue;
        }

        // Testuj czy ruch nie powoduje szacha swojego krola
        Board after = board;
        after.makeMove(move.fromY, move.fromX, move.toY, move.toX, move.promoPiece);
        if (after.isCheck(turn)) {
            std::cout << "Nie mozna zostawic swojego krola pod szachem!\n";
            continue;
        }

        board.makeMove(move.fromY, move.fromX, move.toY, move.toX, move.promoPiece);
        turn = (turn == WHITE) ? BLACK : WHITE;
        board.print();
    }

    return 0;
}
