#include "chess_game.h"
#include <cstdlib>
#include <cstring>
#include <iostream>

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

    if (gameOver) {
        std::cout << "Game over! No more moves allowed.\n";
        return false;
    }

    if (startRow == endRow && startCol == endCol) {
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

    // Prevent capturing own piece
    PieceType target = board[endRow][endCol];
    bool targetIsWhite =
        (target == PieceType::WHITE_PAWN || target == PieceType::WHITE_KNIGHT ||
        target == PieceType::WHITE_BISHOP || target == PieceType::WHITE_ROOK ||
        target == PieceType::WHITE_QUEEN || target == PieceType::WHITE_KING);

    if (target != PieceType::EMPTY && targetIsWhite == isWhitePiece) {
        return false; // can't capture your own team
    }

    PieceType origStart = board[startRow][startCol];
    PieceType origEnd   = board[endRow][endCol];

    // For en passant captured pawn (if any)
    bool willBeEnPassantCapture = false;
    int capturedPawnRow = -1;
    PieceType origCapturedPawn = PieceType::EMPTY;
    if ((movingPiece == PieceType::WHITE_PAWN || movingPiece == PieceType::BLACK_PAWN) &&
        startCol != endCol && board[endRow][endCol] == PieceType::EMPTY) {
        // Potential en passant capture, verify using enPassant target
        if (endRow == enPassantTargetRow && endCol == enPassantTargetCol) {
            willBeEnPassantCapture = true;
            capturedPawnRow = isWhitePiece ? endRow + 1 : endRow - 1;
            origCapturedPawn = board[capturedPawnRow][endCol];
        }
    }

    // Castling logic
    bool willBeCastling = false;
    int rookSrcCol = -1, rookDstCol = -1, rookRow = -1;
    PieceType origRookSrc = PieceType::EMPTY, origRookDst = PieceType::EMPTY;
    if (movingPiece == PieceType::WHITE_KING || movingPiece == PieceType::BLACK_KING) {
        if (std::abs(endCol - startCol) == 2) {
            willBeCastling = true;
            rookRow = startRow;
            if (endCol > startCol) {
                // kingside
                rookSrcCol = 7;
                rookDstCol = 5;
            } else {
                // queenside
                rookSrcCol = 0;
                rookDstCol = 3;
            }
            origRookSrc = board[rookRow][rookSrcCol];
            origRookDst = board[rookRow][rookDstCol];
        }
    }

    if (willBeEnPassantCapture) {
        board[capturedPawnRow][endCol] = PieceType::EMPTY;
    }

    if (willBeCastling) {
        board[rookRow][rookDstCol] = board[rookRow][rookSrcCol]; // move rook
        board[rookRow][rookSrcCol] = PieceType::EMPTY;
    }

    board[endRow][endCol] = movingPiece;
    board[startRow][startCol] = PieceType::EMPTY;

    // Pawn Promotion
    if (movingPiece == PieceType::WHITE_PAWN && endRow == 0) {
        board[endRow][endCol] = PieceType::WHITE_QUEEN; // Promote to queen by default
    }
    else if (movingPiece == PieceType::BLACK_PAWN && endRow == 7) {
        board[endRow][endCol] = PieceType::BLACK_QUEEN; // Promote to queen by default
    }


    bool inCheck = isKingInCheck(isWhitePiece);

    if (inCheck) {
        board[startRow][startCol] = origStart;
        board[endRow][endCol] = origEnd;

        if (willBeEnPassantCapture) {
            board[capturedPawnRow][endCol] = origCapturedPawn;
        }

        if (willBeCastling) {
            board[rookRow][rookSrcCol] = origRookSrc;
            board[rookRow][rookDstCol] = origRookDst;
        }

        return false;
    }

    enPassantTargetRow = -1;
    enPassantTargetCol = -1;
    if ((movingPiece == PieceType::WHITE_PAWN && startRow - endRow == 2) ||
        (movingPiece == PieceType::BLACK_PAWN && endRow - startRow == 2)) {
        enPassantTargetRow = (startRow + endRow) / 2;
        enPassantTargetCol = startCol;
    }

    if (willBeCastling) {
        bool isWhiteKing = (movingPiece == PieceType::WHITE_KING);
        if (isWhiteKing) {
            whiteKingMoved = true;
            if (rookSrcCol == 7) whiteRookKingsideMoved = true;
            else if (rookSrcCol == 0) whiteRookQueensideMoved = true;
        } else {
            blackKingMoved = true;
            if (rookSrcCol == 7) blackRookKingsideMoved = true;
            else if (rookSrcCol == 0) blackRookQueensideMoved = true;
        }
    }

    if (movingPiece == PieceType::WHITE_ROOK) {
        if (startRow == 7 && startCol == 0) whiteRookQueensideMoved = true;
        if (startRow == 7 && startCol == 7) whiteRookKingsideMoved = true;
    }
    if (movingPiece == PieceType::BLACK_ROOK) {
        if (startRow == 0 && startCol == 0) blackRookQueensideMoved = true;
        if (startRow == 0 && startCol == 7) blackRookKingsideMoved = true;
    }

    if (movingPiece == PieceType::WHITE_KING && !willBeCastling) whiteKingMoved = true;
    if (movingPiece == PieceType::BLACK_KING && !willBeCastling) blackKingMoved = true;

    // Switch turn
    whiteTurn = !whiteTurn;

    if (isInsufficientMaterial()) {
        std::cout << "Draw by insufficient material!\n";
        gameOver = true;
    }

    // After move, check if opponent is checkmated
    bool opponentIsWhite = whiteTurn; // we just flipped turns
    if (isCheckmate(opponentIsWhite)) {
        std::cout << (opponentIsWhite ? "White" : "Black") << " is checkmated!\n";
        gameOver = true;
    } else if (isStalemate(opponentIsWhite)) {
        std::cout << "Stalemate! The game is a draw.\n";
        gameOver = true;
    }

    return true;
}


bool ChessGame::isKingInCheck(bool whiteKing) const {
    int kingRow = -1, kingCol = -1;

    // Find the king
    PieceType kingType = whiteKing ? PieceType::WHITE_KING : PieceType::BLACK_KING;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            if (board[r][c] == kingType) {
                kingRow = r;
                kingCol = c;
                break;
            }
        }
    }

    if (kingRow == -1) return false; // should never happen

    // Check if any enemy piece can move to that square
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            PieceType attacker = board[r][c];
            if (attacker == PieceType::EMPTY) continue;

            bool isWhiteAttacker =
                (attacker == PieceType::WHITE_PAWN || attacker == PieceType::WHITE_KNIGHT ||
                 attacker == PieceType::WHITE_BISHOP || attacker == PieceType::WHITE_ROOK ||
                 attacker == PieceType::WHITE_QUEEN || attacker == PieceType::WHITE_KING);

            if (isWhiteAttacker == whiteKing) continue; // skip friendly pieces

            // If this enemy could move to the king's square, it's a check
            bool valid = false;
            if (attacker == PieceType::WHITE_PAWN || attacker == PieceType::BLACK_PAWN)
                valid = pawnValidator->isValidMove(attacker, r, c, kingRow, kingCol, board, -1, -1);
            else if (attacker == PieceType::WHITE_KING || attacker == PieceType::BLACK_KING)
                valid = kingValidator->isValidMove(attacker, r, c, kingRow, kingCol, board, 
                                                  whiteKing ? whiteKingMoved : blackKingMoved,
                                                  whiteKing ? whiteRookKingsideMoved : blackRookKingsideMoved,
                                                  whiteKing ? whiteRookQueensideMoved : blackRookQueensideMoved);
            else if (attacker == PieceType::WHITE_QUEEN || attacker == PieceType::BLACK_QUEEN)
                valid = queenValidator->isValidMove(attacker, r, c, kingRow, kingCol, board);
            else if (attacker == PieceType::WHITE_ROOK || attacker == PieceType::BLACK_ROOK)
                valid = rookValidator->isValidMove(attacker, r, c, kingRow, kingCol, board);
            else if (attacker == PieceType::WHITE_BISHOP || attacker == PieceType::BLACK_BISHOP)
                valid = bishopValidator->isValidMove(attacker, r, c, kingRow, kingCol, board);
            else if (attacker == PieceType::WHITE_KNIGHT || attacker == PieceType::BLACK_KNIGHT)
                valid = knightValidator->isValidMove(attacker, r, c, kingRow, kingCol, board);

            if (valid)
                return true;
        }
    }

    return false;
}

bool ChessGame::isCheckmate(bool whiteKing) {
    // 1. If the king is NOT in check, it’s not checkmate
    if (!isKingInCheck(whiteKing))
        return false;

    // 2. Try every possible move for every piece of this color
    for (int startRow = 0; startRow < 8; ++startRow) {
        for (int startCol = 0; startCol < 8; ++startCol) {
            PieceType piece = board[startRow][startCol];
            if (piece == PieceType::EMPTY) continue;

            bool isWhitePiece = (
                piece == PieceType::WHITE_PAWN || piece == PieceType::WHITE_KNIGHT ||
                piece == PieceType::WHITE_BISHOP || piece == PieceType::WHITE_ROOK ||
                piece == PieceType::WHITE_QUEEN || piece == PieceType::WHITE_KING
            );

            // Skip opponent pieces
            if (isWhitePiece != whiteKing) continue;

            for (int endRow = 0; endRow < 8; ++endRow) {
                for (int endCol = 0; endCol < 8; ++endCol) {
                    if (startRow == endRow && startCol == endCol) continue;

                    bool valid = false;

                    // Use the same logic you use in makeMove
                    if (piece == PieceType::WHITE_PAWN || piece == PieceType::BLACK_PAWN)
                        valid = pawnValidator->isValidMove(piece, startRow, startCol, endRow, endCol, board, -1, -1);
                    else if (piece == PieceType::WHITE_KING || piece == PieceType::BLACK_KING)
                        valid = kingValidator->isValidMove(piece, startRow, startCol, endRow, endCol, board,
                                                           whiteKing ? whiteKingMoved : blackKingMoved,
                                                           whiteKing ? whiteRookKingsideMoved : blackRookKingsideMoved,
                                                           whiteKing ? whiteRookQueensideMoved : blackRookQueensideMoved);
                    else if (piece == PieceType::WHITE_QUEEN || piece == PieceType::BLACK_QUEEN)
                        valid = queenValidator->isValidMove(piece, startRow, startCol, endRow, endCol, board);
                    else if (piece == PieceType::WHITE_ROOK || piece == PieceType::BLACK_ROOK)
                        valid = rookValidator->isValidMove(piece, startRow, startCol, endRow, endCol, board);
                    else if (piece == PieceType::WHITE_BISHOP || piece == PieceType::BLACK_BISHOP)
                        valid = bishopValidator->isValidMove(piece, startRow, startCol, endRow, endCol, board);
                    else if (piece == PieceType::WHITE_KNIGHT || piece == PieceType::BLACK_KNIGHT)
                        valid = knightValidator->isValidMove(piece, startRow, startCol, endRow, endCol, board);

                    if (!valid) continue;

                    // Simulate move
                    PieceType captured = board[endRow][endCol];
                    board[endRow][endCol] = piece;
                    board[startRow][startCol] = PieceType::EMPTY;

                    bool stillInCheck = isKingInCheck(whiteKing);

                    // Undo move
                    board[startRow][startCol] = piece;
                    board[endRow][endCol] = captured;

                    if (!stillInCheck)
                        return false; // Found at least one escape move
                }
            }
        }
    }

    // No move could escape check → checkmate
    return true;
}

bool ChessGame::isStalemate(bool whiteToMove) const {
    // If the player is currently in check, it's not stalemate
    if (isKingInCheck(whiteToMove)) return false;

    // Try every piece of this color
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            PieceType piece = board[r][c];
            if (piece == PieceType::EMPTY) continue;

            bool isWhitePiece =
                (piece == PieceType::WHITE_PAWN || piece == PieceType::WHITE_KNIGHT ||
                 piece == PieceType::WHITE_BISHOP || piece == PieceType::WHITE_ROOK ||
                 piece == PieceType::WHITE_QUEEN || piece == PieceType::WHITE_KING);

            // Skip enemy pieces
            if (isWhitePiece != whiteToMove) continue;

            // Test all 64 squares for any legal move
            for (int endR = 0; endR < 8; ++endR) {
                for (int endC = 0; endC < 8; ++endC) {
                    if (r == endR && c == endC) continue;

                    bool valid = false;

                    // Validate move by piece type
                    if (piece == PieceType::WHITE_PAWN || piece == PieceType::BLACK_PAWN)
                        valid = pawnValidator->isValidMove(piece, r, c, endR, endC, board, -1, -1);
                    else if (piece == PieceType::WHITE_KING || piece == PieceType::BLACK_KING)
                        valid = kingValidator->isValidMove(piece, r, c, endR, endC, board,
                                                            isWhitePiece ? whiteKingMoved : blackKingMoved,
                                                            isWhitePiece ? whiteRookKingsideMoved : blackRookKingsideMoved,
                                                            isWhitePiece ? whiteRookQueensideMoved : blackRookQueensideMoved);
                    else if (piece == PieceType::WHITE_QUEEN || piece == PieceType::BLACK_QUEEN)
                        valid = queenValidator->isValidMove(piece, r, c, endR, endC, board);
                    else if (piece == PieceType::WHITE_ROOK || piece == PieceType::BLACK_ROOK)
                        valid = rookValidator->isValidMove(piece, r, c, endR, endC, board);
                    else if (piece == PieceType::WHITE_BISHOP || piece == PieceType::BLACK_BISHOP)
                        valid = bishopValidator->isValidMove(piece, r, c, endR, endC, board);
                    else if (piece == PieceType::WHITE_KNIGHT || piece == PieceType::BLACK_KNIGHT)
                        valid = knightValidator->isValidMove(piece, r, c, endR, endC, board);

                    if (!valid) continue;

                    // Prevent capturing own piece
                    PieceType target = board[endR][endC];
                    bool targetIsWhite =
                        (target == PieceType::WHITE_PAWN || target == PieceType::WHITE_KNIGHT ||
                         target == PieceType::WHITE_BISHOP || target == PieceType::WHITE_ROOK ||
                         target == PieceType::WHITE_QUEEN || target == PieceType::WHITE_KING);

                    if (target != PieceType::EMPTY && targetIsWhite == isWhitePiece)
                        continue;

                    // Simulate move
                    PieceType backupStart = board[r][c];
                    PieceType backupEnd = board[endR][endC];
                    const_cast<PieceType(&)[8][8]>(board)[endR][endC] = piece;
                    const_cast<PieceType(&)[8][8]>(board)[r][c] = PieceType::EMPTY;

                    bool stillInCheck = isKingInCheck(whiteToMove);

                    // Undo move
                    const_cast<PieceType(&)[8][8]>(board)[r][c] = backupStart;
                    const_cast<PieceType(&)[8][8]>(board)[endR][endC] = backupEnd;

                    if (!stillInCheck) {
                        return false; // Found at least one legal move
                    }
                }
            }
        }
    }

    // No legal moves found, and not in check → stalemate
    return true;
}

bool ChessGame::isInsufficientMaterial() const {
    // Collect piece counts
    int whitePieces = 0, blackPieces = 0;
    int whiteBishops = 0, blackBishops = 0;
    int whiteKnights = 0, blackKnights = 0;

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            PieceType p = board[r][c];
            switch (p) {
                case PieceType::WHITE_PAWN:
                case PieceType::BLACK_PAWN:
                case PieceType::WHITE_ROOK:
                case PieceType::BLACK_ROOK:
                case PieceType::WHITE_QUEEN:
                case PieceType::BLACK_QUEEN:
                    // Any of these pieces means sufficient material exists
                    return false;

                case PieceType::WHITE_BISHOP:
                    whiteBishops++;
                    break;
                case PieceType::BLACK_BISHOP:
                    blackBishops++;
                    break;
                case PieceType::WHITE_KNIGHT:
                    whiteKnights++;
                    break;
                case PieceType::BLACK_KNIGHT:
                    blackKnights++;
                    break;

                default:
                    break;
            }

            // Track total non-king pieces
            if (p != PieceType::EMPTY && p != PieceType::WHITE_KING)
                whitePieces += (p >= PieceType::WHITE_PAWN && p <= PieceType::WHITE_KING);
            if (p != PieceType::EMPTY && p != PieceType::BLACK_KING)
                blackPieces += (p >= PieceType::BLACK_PAWN && p <= PieceType::BLACK_KING);
        }
    }

    // King vs King
    if (whitePieces == 0 && blackPieces == 0)
        return true;

    // King + Bishop vs King
    if ((whiteBishops == 1 && whitePieces == 1 && blackPieces == 0) ||
        (blackBishops == 1 && blackPieces == 1 && whitePieces == 0))
        return true;

    // King + Knight vs King
    if ((whiteKnights == 1 && whitePieces == 1 && blackPieces == 0) ||
        (blackKnights == 1 && blackPieces == 1 && whitePieces == 0))
        return true;

    // King + Bishop vs King + Bishop on same color squares — optional advanced check
    if (whiteBishops == 1 && blackBishops == 1 && whitePieces == 1 && blackPieces == 1)
        return true;

    return false;
}