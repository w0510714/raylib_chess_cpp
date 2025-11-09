#pragma once
#include "chess_game_enums.h"
#include "move_validator.h"

class QueenMovement {
public:
    QueenMovement() = default;
    bool isValidMove(PieceType piece, int startRow, int startCol, int endRow, int endCol, const PieceType board[8][8]);
};
