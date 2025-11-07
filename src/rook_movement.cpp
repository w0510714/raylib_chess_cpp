#include "rook_movement.h"
#include <cmath>

bool RookMovement::isValidMove(PieceType piece, int startRow, int startCol, int endRow, int endCol,
                                const PieceType board[8][8]) {
    // Rook moves in straight lines: either same row or same column
    if (startRow != endRow && startCol != endCol) return false;

    // Determine piece colors
    bool isWhite = (piece == PieceType::WHITE_ROOK);
    PieceType target = board[endRow][endCol];

    // Check path is clear
    if (startRow == endRow) {
        // Moving horizontally
        int step = (endCol > startCol) ? 1 : -1;
        for (int col = startCol + step; col != endCol; col += step) {
            if (board[startRow][col] != PieceType::EMPTY) {
                return false; // Path blocked
            }
        }
    } else {
        // Moving vertically
        int step = (endRow > startRow) ? 1 : -1;
        for (int row = startRow + step; row != endRow; row += step) {
            if (board[row][startCol] != PieceType::EMPTY) {
                return false; // Path blocked
            }
        }
    }

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