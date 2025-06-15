#include <iostream>
#include <string>
#include <vector>
#include <random>
#include "chess.hpp"

using namespace chess;
void print_board(const Board& board) {
    std::string fen = board.getFen();
    auto parts = utils::splitString(fen, ' ');
    std::string rows(parts[0]);

    std::vector<std::string> lines;
    std::string current;
    for (char c : rows) {
        if (c == '/') {
            lines.push_back(current);
            current.clear();
        }
        else {
            current += c;
        }
    }
    lines.push_back(current); 

    std::cout << "\n  +-----------------+\n";
    for (int i = 0; i < 8; ++i) {
        std::cout << 8 - i << " | ";
        for (char c : lines[i]) {
            if (isdigit(c)) {
                int empty = c - '0';
                for (int j = 0; j < empty; ++j)
                    std::cout << ". ";
            }
            else {
                std::cout << c << ' ';
            }
        }
        std::cout << "|\n";
    }
    std::cout << "  +-----------------+\n";
    std::cout << "    a b c d e f g h\n\n";
}

Move find_move_from_input(const std::string& input, const Board& board) {
    Movelist legal_moves;
    movegen::legalmoves(legal_moves, board);

    for (const Move& m : legal_moves) {
        if (uci::moveToUci(m) == input)
            return m;
    }

    return Move(Move::NO_MOVE); 
}

Move get_random_legal_move(const Board& board) {
    Movelist legal_moves;
    movegen::legalmoves(legal_moves, board);

    if (legal_moves.empty()) return Move(Move::NO_MOVE);

    static std::random_device rd;
    static std::mt19937 rng(rd());
    std::uniform_int_distribution<int> dist(0, legal_moves.size() - 1);

    return legal_moves[dist(rng)];
}

int main() {
    Board board;
    std::vector<Move> move_history;

    while (true) {
        print_board(board);

        if (board.sideToMove() == Color::WHITE) {
            std::cout << "Your move (UCI format, e.g. e2e4), or 'undo'/'quit': ";
            std::string input;
            std::cin >> input;

            if (input == "quit") break;

            if (input == "undo") {
                if (move_history.size() >= 2) {
                    board.unmakeMove(move_history.back());
                    move_history.pop_back();
                    board.unmakeMove(move_history.back());
                    move_history.pop_back();
                }
                else {
                    std::cout << "Nothing to undo.\n";
                }
                continue;
            }

            Move move = find_move_from_input(input, board);
            if (move == Move(Move::NO_MOVE)) {
                std::cout << "Invalid or illegal move. Try again.\n";
                continue;
            }

            board.makeMove(move);
            move_history.push_back(move);
        }
        else {
            std::cout << "Computer is thinking...\n";
            Move ai_move = get_random_legal_move(board);

            if (ai_move == Move(Move::NO_MOVE)) {
                std::cout << "Game over. No legal moves.\n";
                break;
            }

            std::cout << "Computer plays: " << uci::moveToUci(ai_move) << "\n";
            board.makeMove(ai_move);
            move_history.push_back(ai_move);
        }
    }

    std::cout << "Game ended.\n";
    return 0;
}
