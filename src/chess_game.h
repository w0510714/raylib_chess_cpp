#pragma once
#include "pawn_movement.h"
#include "king_movement.h"
#include "piece.h"

class ChessGame {
public:
    ChessGame();
    ~ChessGame() = default;

    PieceType getPieceAt(int row, int col) const;
    bool makeMove(int startRow, int startCol, int endRow, int endCol);
    GameStatus getGameStatus() const { return status; }
    bool isWhiteTurn() const { return whiteTurn; }

private:
    PieceType board[8][8];
    bool whiteTurn;
    GameStatus status;
    PawnMovement pawnValidator;
    KingMovement kingValidator;

    void initializeBoard();
};