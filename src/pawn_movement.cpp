#include "pawn_movement.h"
#include <cmath>

bool PawnMovement::isValidMove(PieceType piece, int startRow, int startCol, int endRow, int endCol, const PieceType board[8][8]) const {
    bool isWhite = (piece == PieceType::WHITE_PAWN);
    int direction = isWhite ? -1 : 1; // White moves "up" (row decreases), black moves "down" (row increases)
    int startRank = isWhite ? 6 : 1;  // white pawns start on row 6, black on row 1 (0-indexed)

    // Bounds check (caller should also check)
    if (startRow < 0 || startRow >= 8 || startCol < 0 || startCol >= 8 ||
        endRow < 0 || endRow >= 8 || endCol < 0 || endCol >= 8) return false;

    // Must be moving
    if (startRow == endRow && startCol == endCol) return false;

    int rowDiff = endRow - startRow;
    int colDiff = endCol - startCol;

    PieceType target = board[endRow][endCol];

    // Forward (no capture)
    if (colDiff == 0) {
        // One step
        if (rowDiff == direction && target == PieceType::EMPTY) {
            return true;
        }
        // Two steps from starting rank (both squares must be empty)
        if (rowDiff == 2 * direction && startRow == startRank) {
            int intermediateRow = startRow + direction;
            if (board[intermediateRow][startCol] == PieceType::EMPTY && target == PieceType::EMPTY) {
                return true;
            }
        }
        return false;
    }

    // Diagonal capture (one square diagonally) — only valid if target is opposing piece
    if (std::abs(colDiff) == 1 && rowDiff == direction) {
        if (target == PieceType::EMPTY) return false;

        bool targetIsWhite =
            (target == PieceType::WHITE_PAWN || target == PieceType::WHITE_KNIGHT ||
             target == PieceType::WHITE_BISHOP || target == PieceType::WHITE_ROOK ||
             target == PieceType::WHITE_QUEEN || target == PieceType::WHITE_KING);

        if (isWhite != targetIsWhite) return true;
    }

    // All other cases invalid for now
    return false;
}
