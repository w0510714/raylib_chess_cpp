#pragma once
#include "chess_game_enums.h"
#include "move_validator.h"

class KnightMovement {
public:
    KnightMovement() = default;
    bool isValidMove(PieceType piece, int startRow, int startCol, int endRow, int endCol,
                 const PieceType board[8][8]);
};
