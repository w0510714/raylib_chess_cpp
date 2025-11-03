#include "chess_game.h"
#include "pawn_movement.h"
#include <cstring>

ChessGame::ChessGame() : whiteTurn(true), status(GameStatus::ONGOING) {
    initializeBoard();
    pawnValidatorPtr = new PawnMovement();
}

void ChessGame::initializeBoard() {
    // Empty board first
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            board[row][col] = PieceType::EMPTY;
        }
    }
        
    // Black back rank (row 0) and pawns (row 1)
    board[0][0] = PieceType::BLACK_ROOK;
    board[0][1] = PieceType::BLACK_KNIGHT;
    board[0][2] = PieceType::BLACK_BISHOP;
    board[0][3] = PieceType::BLACK_QUEEN;
    board[0][4] = PieceType::BLACK_KING;
    board[0][5] = PieceType::BLACK_BISHOP;
    board[0][6] = PieceType::BLACK_KNIGHT;
    board[0][7] = PieceType::BLACK_ROOK;
    for (int col = 0; col < 8; ++col) {
        board[1][col] = PieceType::BLACK_PAWN;
    }

    // White pawns (row 6) and back rank (row 7)
    for (int col = 0; col < 8; ++col) {
        board[6][col] = PieceType::WHITE_PAWN;
    }
    board[7][0] = PieceType::WHITE_ROOK;
    board[7][1] = PieceType::WHITE_KNIGHT;
    board[7][2] = PieceType::WHITE_BISHOP;
    board[7][3] = PieceType::WHITE_QUEEN;
    board[7][4] = PieceType::WHITE_KING;
    board[7][5] = PieceType::WHITE_BISHOP;
    board[7][6] = PieceType::WHITE_KNIGHT;
    board[7][7] = PieceType::WHITE_ROOK;

}

PieceType ChessGame::getPieceAt(int row, int col) const {
    if (row < 0 || row >= 8 || col < 0 || col >= 8)
        return PieceType::EMPTY;
    return board[row][col];
}

bool ChessGame::makeMove(int startRow, int startCol, int endRow, int endCol) {
    // Validate bounds
    if (startRow < 0 || startRow >= 8 || startCol < 0 || startCol >= 8 ||
        endRow < 0 || endRow >= 8 || endCol < 0 || endCol >= 8) {
        return false;
    }

    // Check there is a piece
    PieceType movingPiece = board[startRow][startCol];
    if (movingPiece == PieceType::EMPTY) return false;

    // Check piece color matches turn
    bool isWhitePiece = (movingPiece == PieceType::WHITE_PAWN || movingPiece == PieceType::WHITE_KNIGHT ||
                         movingPiece == PieceType::WHITE_BISHOP || movingPiece == PieceType::WHITE_ROOK ||
                         movingPiece == PieceType::WHITE_QUEEN || movingPiece == PieceType::WHITE_KING);

    if (whiteTurn && !isWhitePiece) return false;
    if (!whiteTurn && isWhitePiece) return false;

    // Validate the move depending on piece
    bool valid = false;
    if (movingPiece == PieceType::WHITE_PAWN || movingPiece == PieceType::BLACK_PAWN) {
        valid = pawnValidatorPtr->isValidMove(movingPiece, startRow, startCol, endRow, endCol, board);
    } else {
        // For non-pawn pieces: allow for now (you can add validators later)
        valid = true;
    }

    if (!valid) return false;

    // Make the move
    board[endRow][endCol] = movingPiece;
    board[startRow][startCol] = PieceType::EMPTY;

    // Switch turn
    whiteTurn = !whiteTurn;

    return true;
}
