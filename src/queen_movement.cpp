#include "queen_movement.h"
#include <cmath>

bool QueenMovement::isValidMove(PieceType piece, int startRow, int startCol, int endRow, int endCol, const PieceType board[8][8]) {
    // Calculate differences for vertical, horizontal, diagonal movements
    int rowDiff = std::abs(endRow - startRow);
    int colDiff = std::abs(endCol - startCol);

    // Queen moves like Rook or Bishop
    bool isRookMove = (startRow == endRow || startCol == endCol);
    bool isBishopMove = (rowDiff == colDiff);

    if (!isRookMove && !isBishopMove) {
        return false;
    }

    // Takes from Rook and Bishop logic to check path is clear
    if (isRookMove) {
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
    } else if (isBishopMove) {
        // Moving diagonally
        int rowStep = (endRow > startRow) ? 1 : -1;
        int colStep = (endCol > startCol) ? 1 : -1;
        int row = startRow + rowStep;
        int col = startCol + colStep;
        while (row != endRow && col != endCol) {
            if (board[row][col] != PieceType::EMPTY) {
                return false; // Path blocked
            }
            row += rowStep;
            col += colStep;
        }
    }

    // Determine piece colors
    bool isWhite = (piece == PieceType::WHITE_QUEEN);
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