#include "board.hpp"
#include "move.hpp"
#include "AI.hpp"
#include <iostream>
#include <string>
#include "test.hpp"

enum GameMode { HUMAN_VS_HUMAN = 1, HUMAN_VS_AI = 2, AI_VS_AI = 3 };
enum AiStrength { NEWBIE = 1, CLUB = 2, GRANDMASTER = 3 };

int get_ai_depth(AiStrength level) {
    switch (level) {
    case NEWBIE: return 4;
    case CLUB:   return 6;     
    case GRANDMASTER: return 8;     
    default:     return 4;
    }
}

std::string ai_name(AiStrength level) {
    switch (level) {
    case NEWBIE: return "Newbie";
    case CLUB:   return "Club Player";
    case GRANDMASTER: return "GRANDMASTER";
    default:     return "Novice";
    }
}

int main() {
    test_chess_engine();

    std::cout << "Wybierz tryb gry:\n";
    std::cout << "1. Czlowiek vs Czlowiek\n";
    std::cout << "2. Czlowiek vs Komputer\n";
    std::cout << "3. Komputer vs Komputer\n";
    std::cout << "Twoj wybor: ";

    int mode_choice = 1;
    std::cin >> mode_choice;

    int white_ai = 0, black_ai = 0;
    int white_depth = 2, black_depth = 2;

    if (mode_choice == HUMAN_VS_AI) {
        std::cout << "Wybierz poziom przeciwnika (1. Newbie, 2. Club Player, 3. GRANDMASTER): ";
        std::cin >> black_ai;
        black_depth = get_ai_depth(static_cast<AiStrength>(black_ai));
        std::cout << "Wybrano: " << ai_name(static_cast<AiStrength>(black_ai)) << " (czarne)\n";
    }
    else if (mode_choice == AI_VS_AI) {
        std::cout << "Wybierz poziom bialego AI (1. Newbie, 2. Club Player, 3. GRANDMASTER): ";
        std::cin >> white_ai;
        white_depth = get_ai_depth(static_cast<AiStrength>(white_ai));
        std::cout << "Wybierz poziom czarnego AI (1. Newbie, 2. Club Player, 3. GRANDMASTER): ";
        std::cin >> black_ai;
        black_depth = get_ai_depth(static_cast<AiStrength>(black_ai));
        std::cout << "Wybrano: Biale: " << ai_name(static_cast<AiStrength>(white_ai))
            << " | Czarne: " << ai_name(static_cast<AiStrength>(black_ai)) << "\n";
    }std::cout << "\n";

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

        // Tryb AI vs AI
        if (mode_choice == AI_VS_AI ||
            (mode_choice == HUMAN_VS_AI && turn == BLACK)) {
            int ai_depth = (turn == WHITE) ? white_depth : black_depth;
            Move aiMove = get_best_move_AI(board, ai_depth, turn);
            if (aiMove.fromY == aiMove.toY && aiMove.fromX == aiMove.toX && aiMove.promoPiece == 0) {
                std::cout << "AI nie widzi ruchu.\n";
                break;
            }
            std::cout << (turn == WHITE ? "Biale" : "Czarne") << " AI wybralo ruch: "
                << char('a' + aiMove.fromX) << (aiMove.fromY + 1)
                << char('a' + aiMove.toX) << (aiMove.toY + 1)
                << (aiMove.promoPiece == QUEEN ? "h" : aiMove.promoPiece == ROOK ? "w" : aiMove.promoPiece == KNIGHT ? "g" : aiMove.promoPiece == BISHOP ? "s" : "")
                << std::endl;
            board.makeMove(aiMove.fromY, aiMove.fromX, aiMove.toY, aiMove.toX, aiMove.promoPiece);
            turn = (turn == WHITE) ? BLACK : WHITE;
            board.print();
            continue;
        }

        // Tryb Człowiek vs Człowiek oraz białe w Człowiek vs AI
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
