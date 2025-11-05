#include "bishop_movement.h"
#include <cmath>

bool BishopMovement::isValidMove(PieceType piece, int startRow, int startCol, int endRow, int endCol, PieceType board[8][8]) {
    // Diagonal Movement
    int rowDiff = std::abs(endRow - startRow);
    int colDiff = std::abs(endCol - startCol);
    if (rowDiff != colDiff) {
        return false;
    }

    // Determine piece colors
    bool isWhite = (piece == PieceType::WHITE_BISHOP);
    PieceType target = board[endRow][endCol];

    // Check path is clear
    int rowStep = (endRow > startRow) ? 1 : -1;
    int colStep = (endCol > startCol) ? 1 : -1;
    int currentRow = startRow + rowStep;
    int currentCol = startCol + colStep;
    while (currentRow != endRow && currentCol != endCol) {
        if (board[currentRow][currentCol] != PieceType::EMPTY) {
            return false; // Path blocked
        }
        currentRow += rowStep;
        currentCol += colStep;
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