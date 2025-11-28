#pragma once

#include "chess_game.h"
#include "chess_game_enums.h"
#include "raylib.h"
#include "uci_engine.h"
#include <memory>
#include <string>
#include <vector>


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

  // UCI Engine integration
  std::unique_ptr<UCIEngine> uciEngine;
  std::string startingFen;
  std::vector<std::string> moveHistory;
  EngineAnalysis currentAnalysis;

  // Loads textures for board and pieces
  void renderGame();
  void updateGame();
  void renderUI();
  void renderBoard();
  void renderPieces();
  void renderEngineAnalysis();
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
};
