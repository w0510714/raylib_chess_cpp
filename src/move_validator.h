#pragma once
#include "chess_game_enums.h"  // where PieceType & GameStatus are defined

class MoveValidator {
public:
    virtual ~MoveValidator() = default;

    // Every piece validator implements its own movement logic
    virtual bool isValidMove(PieceType piece, int startRow, int startCol, int endRow, int endCol, const PieceType board[8][8]) const = 0;
};
