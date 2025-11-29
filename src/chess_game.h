#include "chess_game_enums.h"
#include <cmath>
#include <memory>
#include <vector>


class ChessGame {
public:
  ChessGame();
  ~ChessGame() = default;

  PieceType getPieceAt(int row, int col) const;
  bool makeMove(int startRow, int startCol, int endRow, int endCol);
  GameStatus getGameStatus() const { return status; }
  bool isWhiteTurn() const { return whiteTurn; }
  bool isGameOver() const { return gameOver; }
  bool setWhiteTurn(bool isWhite) {
    whiteTurn = isWhite;
    return whiteTurn;
  }
  void initializeBoard();
  bool loadFromFEN(const char* fen);
  std::string generateFEN() const;  // Generate FEN string from current position
  
  // Captured pieces tracking
  std::vector<PieceType> getWhiteCapturedPieces() const { return whiteCapturedPieces; }
  std::vector<PieceType> getBlackCapturedPieces() const { return blackCapturedPieces; }
  
  // Check detection
  bool isKingInCheck(bool whiteKing) const;

private:
  PieceType board[8][8];
  bool whiteTurn;
  GameStatus status;
  bool gameOver = false;

  bool isCheckmate(bool whiteKing);
  bool isStalemate(bool whiteKing) const;
  bool isInsufficientMaterial() const;
  bool isFiftyMoveRuleReached() const;

  // Move Validation Helpers
  bool isValidPawnMove(PieceType piece, int startRow, int startCol, int endRow,
                       int endCol, const PieceType board[8][8],
                       int enPassantTargetRow, int enPassantTargetCol) const;
  bool isValidKingMove(PieceType piece, int startRow, int startCol, int endRow,
                       int endCol, const PieceType board[8][8], bool kingMoved,
                       bool rookKingsideMoved, bool rookQueensideMoved) const;
  bool isValidQueenMove(PieceType piece, int startRow, int startCol, int endRow,
                        int endCol, const PieceType board[8][8]) const;
  bool isValidRookMove(PieceType piece, int startRow, int startCol, int endRow,
                       int endCol, const PieceType board[8][8]) const;
  bool isValidBishopMove(PieceType piece, int startRow, int startCol,
                         int endRow, int endCol,
                         const PieceType board[8][8]) const;
  bool isValidKnightMove(PieceType piece, int startRow, int startCol,
                         int endRow, int endCol,
                         const PieceType board[8][8]) const;

  int enPassantTargetRow = -1;
  int enPassantTargetCol = -1;
  int halfMoveClock = 0;

  bool whiteKingMoved = false;
  bool blackKingMoved = false;
  bool whiteRookKingsideMoved = false;
  bool whiteRookQueensideMoved = false;
  bool blackRookKingsideMoved = false;
  bool blackRookQueensideMoved = false;
  
  // Captured pieces tracking
  std::vector<PieceType> whiteCapturedPieces;  // Pieces captured by black
  std::vector<PieceType> blackCapturedPieces;  // Pieces captured by white

};
