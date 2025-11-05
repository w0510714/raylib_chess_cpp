#pragma once
#include "pawn_movement.h"
#include "king_movement.h"
#include "chess_game_enums.h"
#include <memory>

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
    std::unique_ptr<PawnMovement> pawnValidator;
    std::unique_ptr<KingMovement> kingValidator;

    void initializeBoard();
};