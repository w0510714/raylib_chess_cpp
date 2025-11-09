#include "pawn_movement.h"
#include <cstdlib>

bool PawnMovement::isValidMove(
    PieceType piece,
    int startRow, int startCol,
    int endRow, int endCol,
    const PieceType board[8][8],
    int enPassantTargetRow, int enPassantTargetCol
) const {
    bool isWhite = (piece == PieceType::WHITE_PAWN);
    int direction = isWhite ? -1 : 1;
    int startRank = isWhite ? 6 : 1;

    if (startRow < 0 || startRow >= 8 || startCol < 0 || startCol >= 8 ||
        endRow < 0 || endRow >= 8 || endCol < 0 || endCol >= 8)
        return false;

    if (startRow == endRow && startCol == endCol) return false;

    int rowDiff = endRow - startRow;
    int colDiff = endCol - startCol;
    PieceType target = board[endRow][endCol];

    // Forward Move
    if (colDiff == 0) {
        if (rowDiff == direction && target == PieceType::EMPTY)
            return true;
        if (rowDiff == 2 * direction && startRow == startRank) {
            int midRow = startRow + direction;
            if (board[midRow][startCol] == PieceType::EMPTY && target == PieceType::EMPTY)
                return true;
        }
        return false;
    }

    //Diagonal Capture or En Passant
    if (std::abs(colDiff) == 1 && rowDiff == direction) {
        // Regular capture
        bool targetIsWhite =
            (target == PieceType::WHITE_PAWN || target == PieceType::WHITE_KNIGHT ||
             target == PieceType::WHITE_BISHOP || target == PieceType::WHITE_ROOK ||
             target == PieceType::WHITE_QUEEN || target == PieceType::WHITE_KING);

        if (target != PieceType::EMPTY && isWhite != targetIsWhite)
            return true;

        // En passant capture
        if (target == PieceType::EMPTY &&
            endRow == enPassantTargetRow && endCol == enPassantTargetCol)
            return true;
    }

    return false;
}
