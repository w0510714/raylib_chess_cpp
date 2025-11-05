#pragma once
#include "chess_game_enums.h"
#include "move_validator.h"

class RookMovement {
public:
    RookMovement() = default;
    bool isValidMove(PieceType piece, int startRow, int startCol, int endRow, int endCol, PieceType board[8][8]);
};
