#include "king_movement.h"
#include <cstdlib>

bool KingMovement::isValidMove(
    PieceType piece,
    int startRow, int startCol,
    int endRow, int endCol,
    const PieceType board[8][8],
    bool kingMoved, bool rookKingsideMoved, bool rookQueensideMoved
) const {
    int rowDiff = std::abs(endRow - startRow);
    int colDiff = std::abs(endCol - startCol);

    // Normal king move: 1 square any direction
    if (rowDiff <= 1 && colDiff <= 1)
        return true;

    // ==== CASTLING ====
    bool isWhite = (piece == PieceType::WHITE_KING);
    if (rowDiff == 0 && colDiff == 2 && !kingMoved) {
        // Kingside castling
        if (endCol > startCol && !rookKingsideMoved) {
            if (board[startRow][5] == PieceType::EMPTY && board[startRow][6] == PieceType::EMPTY)
                return true;
        }
        // Queenside castling
        if (endCol < startCol && !rookQueensideMoved) {
            if (board[startRow][1] == PieceType::EMPTY && board[startRow][2] == PieceType::EMPTY && board[startRow][3] == PieceType::EMPTY)
                return true;
        }
    }

    return false;
}
