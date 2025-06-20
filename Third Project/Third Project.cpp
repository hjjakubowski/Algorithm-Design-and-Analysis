#include "board.hpp"
#include "bitboard_tables.hpp"
#include "move.hpp"
#include "AI.hpp"
#include "test.hpp"
#include "zorbist.hpp"
#include <iostream>
#include <string>
#include <vector>

enum GameMode { HUMAN_VS_HUMAN = 1, HUMAN_VS_AI = 2, AI_VS_AI = 3 };
enum AiStrength { NEWBIE = 1, CLUB = 2, GRANDMASTER = 3 };

int get_ai_depth(AiStrength level) {
    switch (level) {
    case NEWBIE: return 2;
    case CLUB:   return 8;
    case GRANDMASTER: return 16;
    default:     return 2;
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

void print_move(const Move& move) {
    char fromX = 'a' + (move.from % 8);
    char fromY = '1' + (move.from / 8);
    char toX = 'a' + (move.to % 8);
    char toY = '1' + (move.to / 8);

    std::cout << fromX << fromY << toX << toY;
    if (move.promoPiece != NONE_TYPE) {
        switch (move.promoPiece) {
        case QUEEN:  std::cout << "q"; break;
        case ROOK:   std::cout << "r"; break;
        case KNIGHT: std::cout << "n"; break;
        case BISHOP: std::cout << "b"; break;
        default: break;
        }
    }
}

int main() {
    init_knight_attacks();
    init_king_attacks();
    //test_chess_engine();
    init_zobrist();
    transTable.clear();

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
    }
    std::cout << "\n";

    Board board;
    PieceColor turn = WHITE;
    board.setInitial();
    board.print();

    while (true) {
        if (board.isInsufficientMaterial()) {
			std::cout << "Remis! Brak mozliwosci zamatowania przeciwnika.\n";
			break;
        }
        if (board.halfmoveClock >= 100) {
            std::cout << "Remis! (50 ruchow bez bicia)\n";
            board.print();
            break;
        }

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

        // AI vs AI lub AI (czarne) vs Człowiek
        if (mode_choice == AI_VS_AI ||
            (mode_choice == HUMAN_VS_AI && turn == BLACK)) {
            int ai_depth = (turn == WHITE) ? white_depth : black_depth;
            Move aiMove = get_best_move_AI(board, ai_depth, turn);
            if (aiMove.from == aiMove.to && aiMove.promoPiece == NONE_TYPE) {
                std::cout << "AI nie widzi ruchu.\n";
                break;
            }
            std::cout << (turn == WHITE ? "Biale" : "Czarne") << " AI wybralo ruch: ";
            print_move(aiMove);
            std::cout << std::endl;
            board.makeMove(aiMove, turn);
            
            turn = (turn == WHITE) ? BLACK : WHITE;
            board.print();
            continue;
        }

        // Człowiek vs Człowiek lub białe w Człowiek vs AI
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
        if (move.from < 0 || move.from > 63 || move.to < 0 || move.to > 63) {
            std::cout << "Niepoprawne pole!\n";
            continue;
        }

        if (!board.isLegalMove(move, turn)) {
            std::cout << "Nielegalny ruch!\n";
            continue;
        }

        board.makeMove(move, turn);
        turn = (turn == WHITE) ? BLACK : WHITE;
        board.print();

        if (board.halfmoveClock >= 100) {
            std::cout << "Remis! (50 ruchow bez bicia)\n";
            break;
        }

    }

    return 0;
}
