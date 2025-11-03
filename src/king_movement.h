#pragma once
#include "piece.h"

class KingMovement {
public:
    KingMovement() = default;
    bool isValidMove(PieceType piece, int startRow, int startCol, int endRow, int endCol, PieceType board[8][8]);
};
