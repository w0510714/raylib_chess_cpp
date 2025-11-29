#pragma once

#include "chess_game.h"
#include "chess_game_enums.h"
#include "raylib.h"
#include "uci_engine.h"
#include <memory>
#include <string>
#include <vector>

// Structure to track individual moves
struct Move {
  std::string algebraic;        // Move in algebraic notation (e.g., "e2e4")
  std::string sanNotation;      // Standard Algebraic Notation (e.g., "e4")
  int moveNumber;               // Full move number (1, 2, 3, ...)
  bool isWhiteMove;             // true if white played this move
};

class ChessAnalysisProgram {
public:
  ChessAnalysisProgram();
  ~ChessAnalysisProgram();

  void run();

private:
  Texture2D boardTexture;
  Texture2D whiteKnightTexture;
  Texture2D whiteBishopTexture;
  Texture2D whiteRookTexture;
  Texture2D whiteKingTexture;
  Texture2D whiteQueenTexture;
  Texture2D whitePawnTexture;
  Texture2D blackKnightTexture;
  Texture2D blackBishopTexture;
  Texture2D blackRookTexture;
  Texture2D blackKingTexture;
  Texture2D blackQueenTexture;
  Texture2D blackPawnTexture;

  ChessGame currentPosition;
  ChessGame game; // For later

  // Drag and drop state
  bool dragging = false;
  int dragRow = -1, dragCol = -1;
  Vector2 dragOffset = {0, 0};
  PieceType draggedPiece = PieceType::EMPTY;
  bool flipBoard = false; // Visual flip: when true, draw board upside-down

  // UCI Engine integration
  std::unique_ptr<UCIEngine> uciEngine;
  std::string startingFen;
  std::string currentBaseFen;  // Current base FEN (standard or loaded FEN)
  std::vector<Move> moveHistory;
  EngineAnalysis currentAnalysis;
  std::string lastEnginePositionFen;  // Track position to detect changes

  void renderGame();
  void updateGame();
  void renderUI();
  void renderBoard();
  void renderPieces();
  void renderEngineAnalysis();
  void renderMoveHistory();
  void renderControls();
  void getTextureForPiece();
  void loadAllTextures();
  void unloadAllTextures();
  Texture2D getTextureForPiece(PieceType Place);

  // UCI Engine helpers
  void updateEnginePosition();
  std::string moveToAlgebraic(int startRow, int startCol, int endRow,
                              int endCol, PieceType piece);
  std::string parseEvaluation(const std::string &infoLine);
  std::string parseDepth(const std::string &infoLine);
  std::string parsePV(const std::string &infoLine);
  void addMoveToHistory(const std::string &algebraic, const std::string &san);
};
