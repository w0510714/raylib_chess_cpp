#include "knight_movement.h"
#include <cmath>

bool KnightMovement::isValidMove(PieceType piece, int startRow, int startCol, int endRow, int endCol, PieceType board[8][8]) {
    int rowDiff = std::abs(endRow - startRow);
    int colDiff = std::abs(endCol - startCol);

    // Knight moves in L-shape: 2 by 1 or 1 by 2
    if (!((rowDiff == 2 && colDiff == 1) || (rowDiff == 1 && colDiff == 2))) {
        return false;
    }

    // Determine piece colors
    bool isWhite = (piece == PieceType::WHITE_KNIGHT);
    PieceType target = board[endRow][endCol];

    // If target square is empty, move is fine
    if (target == PieceType::EMPTY) {
        return true;
    }

    // Otherwise, can only capture opposing color
    bool targetIsWhite = (
        target == PieceType::WHITE_PAWN || target == PieceType::WHITE_KNIGHT || target == PieceType::WHITE_BISHOP ||
        target == PieceType::WHITE_ROOK || target == PieceType::WHITE_QUEEN || target == PieceType::WHITE_KING
    );

    return (isWhite != targetIsWhite);
}