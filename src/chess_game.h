#pragma once
#include "move_validator.h"
#include "pawn_movement.h"
#include "king_movement.h"
#include "queen_movement.h"
#include "rook_movement.h"
#include "bishop_movement.h"
#include "knight_movement.h"
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
    bool isGameOver() const { return gameOver; }

private:
    PieceType board[8][8];
    bool whiteTurn;
    GameStatus status;
    bool isKingInCheck(bool whiteKing) const;
    bool isCheckmate(bool whiteKing);
    bool gameOver = false;
    std::unique_ptr<PawnMovement> pawnValidator;
    std::unique_ptr<KingMovement> kingValidator;
    std::unique_ptr<QueenMovement> queenValidator;
    std::unique_ptr<RookMovement> rookValidator;
    std::unique_ptr<BishopMovement> bishopValidator;
    std::unique_ptr<KnightMovement> knightValidator;

    int enPassantTargetRow = -1;
    int enPassantTargetCol = -1;

    bool whiteKingMoved = false;
    bool blackKingMoved = false;
    bool whiteRookKingsideMoved = false;
    bool whiteRookQueensideMoved = false;
    bool blackRookKingsideMoved = false;
    bool blackRookQueensideMoved = false;


    void initializeBoard();
};