#include "king_movement.h"
#include <cmath>

bool KingMovement::isValidMove(PieceType piece, int startRow, int startCol, int endRow, int endCol, PieceType board[8][8]) {
    int rowDiff = std::abs(endRow - startRow);
    int colDiff = std::abs(endCol - startCol);

    // Cannot stay in place
    if (rowDiff == 0 && colDiff == 0)
        return false;

    // King moves only one square in any direction
    if (rowDiff > 1 || colDiff > 1)
        return false;

    // Determine piece colors
    bool isWhite = (piece == PieceType::WHITE_KING);
    PieceType target = board[endRow][endCol];

    // If target square is empty, move is fine
    if (target == PieceType::EMPTY)
        return true;

    // Otherwise, can only capture opposing color
    bool targetIsWhite = (
        target == PieceType::WHITE_PAWN || target == PieceType::WHITE_KNIGHT || target == PieceType::WHITE_BISHOP ||
        target == PieceType::WHITE_ROOK || target == PieceType::WHITE_QUEEN || target == PieceType::WHITE_KING
    );

    return (isWhite != targetIsWhite);
}
