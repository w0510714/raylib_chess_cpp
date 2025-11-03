#pragma once
#include "piece.h"

class PawnMovement {
public:
    PawnMovement() = default;

    // board passed as const reference to array
    bool isValidMove(PieceType piece, int startRow, int startCol, int endRow, int endCol, const PieceType board[8][8]) const;
};
