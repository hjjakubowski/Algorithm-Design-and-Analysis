#include "piece.hpp"

char pieceToChar(uint8_t piece) {
    PieceType type = getPieceType(piece);
    PieceColor color = getPieceColor(piece);
    switch (type) {
    case PAWN:   return (color == WHITE) ? 'P' : (color == BLACK ? 'p' : '.');
    case ROOK:   return (color == WHITE) ? 'R' : (color == BLACK ? 'r' : '.');
    case KNIGHT: return (color == WHITE) ? 'N' : (color == BLACK ? 'n' : '.');
    case BISHOP: return (color == WHITE) ? 'B' : (color == BLACK ? 'b' : '.');
    case QUEEN:  return (color == WHITE) ? 'Q' : (color == BLACK ? 'q' : '.');
    case KING:   return (color == WHITE) ? 'K' : (color == BLACK ? 'k' : '.');
    default:     return '.';
    }
}
