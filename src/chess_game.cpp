#include "chess_game.h"
#include <cstdlib>
#include <cstring>

ChessGame::ChessGame() : whiteTurn(true), status(GameStatus::ONGOING) {
    initializeBoard();
    std::unique_ptr<MoveValidator> pawnValidator;
    std::unique_ptr<MoveValidator> kingValidator;
    std::unique_ptr<MoveValidator> queenValidator;
    std::unique_ptr<MoveValidator> rookValidator;
    std::unique_ptr<MoveValidator> bishopValidator;
    std::unique_ptr<MoveValidator> knightValidator;
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

    PieceType movingPiece = board[startRow][startCol];
    if (movingPiece == PieceType::EMPTY) return false;

    bool isWhitePiece = (movingPiece == PieceType::WHITE_PAWN || movingPiece == PieceType::WHITE_KNIGHT ||
                         movingPiece == PieceType::WHITE_BISHOP || movingPiece == PieceType::WHITE_ROOK ||
                         movingPiece == PieceType::WHITE_QUEEN || movingPiece == PieceType::WHITE_KING);

    if (whiteTurn && !isWhitePiece) return false;
    if (!whiteTurn && isWhitePiece) return false;

    bool valid = false;

    if (movingPiece == PieceType::WHITE_PAWN || movingPiece == PieceType::BLACK_PAWN) {
        valid = pawnValidator->isValidMove(movingPiece, startRow, startCol, endRow, endCol,
                                           board, enPassantTargetRow, enPassantTargetCol);
    } else if (movingPiece == PieceType::WHITE_KING || movingPiece == PieceType::BLACK_KING) {
        valid = kingValidator->isValidMove(movingPiece, startRow, startCol, endRow, endCol, board, 
                                           isWhitePiece ? whiteKingMoved : blackKingMoved,
                                           isWhitePiece ? whiteRookKingsideMoved : blackRookKingsideMoved,
                                           isWhitePiece ? whiteRookQueensideMoved : blackRookQueensideMoved);
    } else if (movingPiece == PieceType::WHITE_QUEEN || movingPiece == PieceType::BLACK_QUEEN) {
        valid = queenValidator->isValidMove(movingPiece, startRow, startCol, endRow, endCol, board);
    } else if (movingPiece == PieceType::WHITE_ROOK || movingPiece == PieceType::BLACK_ROOK) {
        valid = rookValidator->isValidMove(movingPiece, startRow, startCol, endRow, endCol, board);
    } else if (movingPiece == PieceType::WHITE_BISHOP || movingPiece == PieceType::BLACK_BISHOP) {
        valid = bishopValidator->isValidMove(movingPiece, startRow, startCol, endRow, endCol, board);
    } else if (movingPiece == PieceType::WHITE_KNIGHT || movingPiece == PieceType::BLACK_KNIGHT) {
        valid = knightValidator->isValidMove(movingPiece, startRow, startCol, endRow, endCol, board);
    }

    if (!valid) return false;

    // ====== PART 1: Handle En Passant Capture ======
    if ((movingPiece == PieceType::WHITE_PAWN || movingPiece == PieceType::BLACK_PAWN) &&
        endCol != startCol && board[endRow][endCol] == PieceType::EMPTY) {
        int capturedPawnRow = isWhitePiece ? endRow + 1 : endRow - 1;
        board[capturedPawnRow][endCol] = PieceType::EMPTY;
    }

    // ====== PART 2: Move Piece ======
    board[endRow][endCol] = movingPiece;
    board[startRow][startCol] = PieceType::EMPTY;

    // ====== PART 3: Update En Passant Target ======
    enPassantTargetRow = -1;
    enPassantTargetCol = -1;

    if ((movingPiece == PieceType::WHITE_PAWN && startRow - endRow == 2) ||
        (movingPiece == PieceType::BLACK_PAWN && endRow - startRow == 2)) {
        enPassantTargetRow = (startRow + endRow) / 2;
        enPassantTargetCol = startCol;
    }

    // ===== CASTLING LOGIC =====
    if (movingPiece == PieceType::WHITE_KING || movingPiece == PieceType::BLACK_KING) {
        bool isWhiteKing = (movingPiece == PieceType::WHITE_KING);

        // Kingside castle
        if (std::abs(endCol - startCol) == 2 && endCol > startCol) {
            int row = startRow;
            board[row][5] = isWhiteKing ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK;
            board[row][7] = PieceType::EMPTY;
        }

        // Queenside castle
        if (std::abs(endCol - startCol) == 2 && endCol < startCol) {
            int row = startRow;
            board[row][3] = isWhiteKing ? PieceType::WHITE_ROOK : PieceType::BLACK_ROOK;
            board[row][0] = PieceType::EMPTY;
        }

        // Record that king has moved
        if (isWhiteKing) whiteKingMoved = true;
        else blackKingMoved = true;
    }

    // ===== ROOK MOVE TRACKING =====
    if (movingPiece == PieceType::WHITE_ROOK) {
        if (startRow == 7 && startCol == 0) whiteRookQueensideMoved = true;
        if (startRow == 7 && startCol == 7) whiteRookKingsideMoved = true;
    }
    if (movingPiece == PieceType::BLACK_ROOK) {
        if (startRow == 0 && startCol == 0) blackRookQueensideMoved = true;
        if (startRow == 0 && startCol == 7) blackRookKingsideMoved = true;
    }


    // ====== PART 4: Switch Turn ======
    whiteTurn = !whiteTurn;
    return true;
}


