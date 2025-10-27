#include "chess_game.h"

ChessGame::ChessGame() : whiteTurn(true), status(GameStatus::ONGOING) {
    initializeBoard();
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
    if (startRow < 0 || startRow >= 8 || startCol < 0 || startCol >= 8 ||
        endRow < 0 || endRow >= 8 || endCol < 0 || endCol >= 8)
        return false;

    // Just move visually, no rules yet
    board[endRow][endCol] = board[startRow][startCol];
    board[startRow][startCol] = PieceType::EMPTY;
    return true;
}