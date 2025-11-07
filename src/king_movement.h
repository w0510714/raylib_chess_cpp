#ifndef KING_MOVEMENT_H
#define KING_MOVEMENT_H

#include "chess_game_enums.h"
#include "move_validator.h"

class KingMovement {
public:
    bool isValidMove(
        PieceType piece,
        int startRow, int startCol,
        int endRow, int endCol,
        const PieceType board[8][8],
        bool kingMoved,
        bool rookKingsideMoved,
        bool rookQueensideMoved
    ) const;
};

#endif