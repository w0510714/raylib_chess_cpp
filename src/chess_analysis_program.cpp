#include "chess_analysis_program.h"
#include <raylib.h>
#include <regex>
#include <sstream>

ChessAnalysisProgram::ChessAnalysisProgram() {
  // Initialization
  const int screenWidth = 1920;
  const int screenHeight = 1080;

  InitWindow(screenWidth, screenHeight, "Chess Analysis Program");

  SetTargetFPS(120); // Set the desired frame rate

  // Load textures after window creation (some platforms require an OpenGL
  // context)
  loadAllTextures();

  // Initialize UCI engine (disabled by default)
  // Update this path to point to your Stockfish executable
  uciEngine = std::make_unique<UCIEngine>("src/stockfish/stockfish.exe");

  // Set starting FEN (standard starting position)
  startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  moveHistory.clear();
}

ChessAnalysisProgram::~ChessAnalysisProgram() {
  unloadAllTextures();
  CloseWindow(); // Close window and OpenGL context
}

void ChessAnalysisProgram::loadAllTextures() {
  // Load assets
  // CHANGED: Use woodenboard.png instead of board.png
  this->boardTexture = LoadTexture("src/assets/board.png");
  if (!this->boardTexture.id) {
    TraceLog(LOG_ERROR, "Failed to load board texture!");
  } else {
    TraceLog(LOG_INFO, "Board texture loaded: %dx%d", this->boardTexture.width,
             this->boardTexture.height);
  }

  // Note: filenames are assumed to be present in src/assets/chess_pieces/ or
  // images/chess_pieces/ (download and place them accordingly)
  this->whiteKnightTexture = LoadTexture("src/images/chess_pieces/wn.png");
  if (!this->whiteKnightTexture.id) {
    TraceLog(LOG_ERROR, "Failed to load white knight texture!");
  } else {
    TraceLog(LOG_INFO, "White Knight texture loaded: %dx%d",
             this->whiteKnightTexture.width, this->whiteKnightTexture.height);
  }

  this->whiteBishopTexture = LoadTexture("src/images/chess_pieces/wb.png");
  if (!this->whiteBishopTexture.id) {
    TraceLog(LOG_ERROR, "Failed to load white bishop texture!");
  }

  this->whiteRookTexture = LoadTexture("src/images/chess_pieces/wr.png");
  if (!this->whiteRookTexture.id) {
    TraceLog(LOG_ERROR, "Failed to load white rook texture!");
  }

  this->whiteKingTexture = LoadTexture("src/images/chess_pieces/wk.png");
  if (!this->whiteKingTexture.id) {
    TraceLog(LOG_ERROR, "Failed to load white king texture!");
  }

  this->whiteQueenTexture = LoadTexture("src/images/chess_pieces/wq.png");
  if (!this->whiteQueenTexture.id) {
    TraceLog(LOG_ERROR, "Failed to load white queen texture!");
  }

  this->whitePawnTexture = LoadTexture("src/images/chess_pieces/wp.png");
  if (!this->whitePawnTexture.id) {
    TraceLog(LOG_ERROR, "Failed to load white pawn texture!");
  }

  this->blackKnightTexture = LoadTexture("src/images/chess_pieces/bn.png");
  if (!this->blackKnightTexture.id) {
    TraceLog(LOG_ERROR, "Failed to load black knight texture!");
  }

  this->blackBishopTexture = LoadTexture("src/images/chess_pieces/bb.png");
  if (!this->blackBishopTexture.id) {
    TraceLog(LOG_ERROR, "Failed to load black bishop texture!");
  }

  this->blackRookTexture = LoadTexture("src/images/chess_pieces/br.png");
  if (!this->blackRookTexture.id) {
    TraceLog(LOG_ERROR, "Failed to load black rook texture!");
  }

  this->blackKingTexture = LoadTexture("src/images/chess_pieces/bk.png");
  if (!this->blackKingTexture.id) {
    TraceLog(LOG_ERROR, "Failed to load black king texture!");
  }

  this->blackQueenTexture = LoadTexture("src/images/chess_pieces/bq.png");
  if (!this->blackQueenTexture.id) {
    TraceLog(LOG_ERROR, "Failed to load black queen texture!");
  }

  this->blackPawnTexture = LoadTexture("src/images/chess_pieces/bp.png");
  if (!this->blackPawnTexture.id) {
    TraceLog(LOG_ERROR, "Failed to load black pawn texture!");
  }
}

void ChessAnalysisProgram::unloadAllTextures() {
  // Unload all textures if loaded
  if (boardTexture.id)
    UnloadTexture(boardTexture);
  if (whiteKnightTexture.id)
    UnloadTexture(whiteKnightTexture);
  if (whiteBishopTexture.id)
    UnloadTexture(whiteBishopTexture);
  if (whiteRookTexture.id)
    UnloadTexture(whiteRookTexture);
  if (whiteKingTexture.id)
    UnloadTexture(whiteKingTexture);
  if (whiteQueenTexture.id)
    UnloadTexture(whiteQueenTexture);
  if (whitePawnTexture.id)
    UnloadTexture(whitePawnTexture);
  if (blackKnightTexture.id)
    UnloadTexture(blackKnightTexture);
  if (blackBishopTexture.id)
    UnloadTexture(blackBishopTexture);
  if (blackRookTexture.id)
    UnloadTexture(blackRookTexture);
  if (blackKingTexture.id)
    UnloadTexture(blackKingTexture);
  if (blackQueenTexture.id)
    UnloadTexture(blackQueenTexture);
  if (blackPawnTexture.id)
    UnloadTexture(blackPawnTexture);
}

void ChessAnalysisProgram::renderUI() {
  // Render user interface elements here
}

void ChessAnalysisProgram::renderBoard() {
  // Calculate scale to fit board perfectly in window
  float scale = GetScreenHeight() / (float)boardTexture.height;
  DrawTextureEx(this->boardTexture, {0, 0}, 0.0f, scale, WHITE);
}

void ChessAnalysisProgram::renderPieces() {
  float boardScale = GetScreenHeight() / (float)boardTexture.height;
  float squareSize = (boardTexture.width * boardScale) / 8.0f;
  float pieceScale = boardScale * 1.0f;
  float pieceOffset =
      (squareSize - (pieceScale * boardTexture.width / 10.5f)) / 2.0f;

  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      PieceType piece = currentPosition.getPieceAt(row, col);
      if (piece == PieceType::EMPTY)
        continue;

      // Skip drawing the dragged piece (so we draw it separately)
      if (dragging && row == dragRow && col == dragCol)
        continue;

      Texture2D tex = getTextureForPiece(piece);
      Vector2 pos = {col * squareSize + pieceOffset,
                     row * squareSize + pieceOffset};
      DrawTextureEx(tex, pos, 0.0f, pieceScale, WHITE);
    }
  }

  // Draw dragged piece under cursor
  if (dragging && draggedPiece != PieceType::EMPTY) {
    Texture2D tex = getTextureForPiece(draggedPiece);
    Vector2 pos = GetMousePosition();
    pos.x += dragOffset.x + pieceOffset;
    pos.y += dragOffset.y + pieceOffset;
    DrawTextureEx(tex, pos, 0.0f, pieceScale, WHITE);
  }
}

Texture2D ChessAnalysisProgram::getTextureForPiece(PieceType piece) {
  switch (piece) {
  case PieceType::WHITE_PAWN:
    return whitePawnTexture;
  case PieceType::WHITE_KNIGHT:
    return whiteKnightTexture;
  case PieceType::WHITE_BISHOP:
    return whiteBishopTexture;
  case PieceType::WHITE_ROOK:
    return whiteRookTexture;
  case PieceType::WHITE_QUEEN:
    return whiteQueenTexture;
  case PieceType::WHITE_KING:
    return whiteKingTexture;
  case PieceType::BLACK_PAWN:
    return blackPawnTexture;
  case PieceType::BLACK_KNIGHT:
    return blackKnightTexture;
  case PieceType::BLACK_BISHOP:
    return blackBishopTexture;
  case PieceType::BLACK_ROOK:
    return blackRookTexture;
  case PieceType::BLACK_QUEEN:
    return blackQueenTexture;
  case PieceType::BLACK_KING:
    return blackKingTexture;
  default:
    return Texture2D{};
  }
}

void ChessAnalysisProgram::updateGame() {
  float boardScale = GetScreenHeight() / (float)boardTexture.height;
  float squareSize = (boardTexture.width * boardScale) / 8.0f;
  float pieceScale = boardScale * 1.0f;
  Vector2 mousePos = GetMousePosition();

  // Calculate board-relative position
  int col = static_cast<int>(mousePos.x / squareSize);
  int row = static_cast<int>(mousePos.y / squareSize);

  // Check for X key press to toggle engine
  if (IsKeyPressed(KEY_X)) {
    if (uciEngine->isEnabled()) {
      uciEngine->disable();
      TraceLog(LOG_INFO, "UCI Engine disabled");
    } else {
      uciEngine->enable();
      updateEnginePosition();
      TraceLog(LOG_INFO, "UCI Engine enabled");
    }
  }

  if (IsKeyPressed(KEY_R)) {
    // Reset game
    currentPosition.initializeBoard();
    moveHistory.clear();
    // Set to white to move
    currentPosition.setWhiteTurn(true);
    // GameStatus Reset
    GameStatus status = GameStatus::ONGOING;
    TraceLog(LOG_INFO, "Game reset to starting position");
    // Update engine position if enabled
    if (uciEngine->isEnabled()) {
      updateEnginePosition();
    }
  }

  if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_U)) {
    if (!moveHistory.empty()) {
      // Undo last move
      moveHistory.pop_back();
      currentPosition.initializeBoard();
      currentPosition.setWhiteTurn(true);
      
      // Replay all moves except the last one
      for (const auto& move : moveHistory) {
        // Parse and replay move from algebraic notation
        int startCol = move.algebraic[0] - 'a';
        int startRow = 8 - (move.algebraic[1] - '0');
        int endCol = move.algebraic[2] - 'a';
        int endRow = 8 - (move.algebraic[3] - '0');
        currentPosition.makeMove(startRow, startCol, endRow, endCol);
      }
      
      // Update engine position if enabled
      if (uciEngine->isEnabled()) {
        updateEnginePosition();
      }
      
      TraceLog(LOG_INFO, "Move undone");
    }
  }

  // Poll engine analysis if enabled
  if (uciEngine->isEnabled()) {
    currentAnalysis = uciEngine->pollAnalysis();
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (row >= 0 && row < 8 && col >= 0 &&
        col < 8) { // Make sure we're clicking within the board
      PieceType piece = currentPosition.getPieceAt(row, col);
      if (piece != PieceType::EMPTY) {
        bool isWhitePiece =
            (piece == PieceType::WHITE_PAWN ||
             piece == PieceType::WHITE_KNIGHT ||
             piece == PieceType::WHITE_BISHOP ||
             piece == PieceType::WHITE_ROOK ||
             piece == PieceType::WHITE_QUEEN || piece == PieceType::WHITE_KING);
        // Only allow dragging pieces that belong to the current player
        if ((currentPosition.isWhiteTurn() && isWhitePiece) ||
            (!currentPosition.isWhiteTurn() && !isWhitePiece)) {
          dragging = true;
          dragRow = row;
          dragCol = col;
          draggedPiece = piece;
          dragOffset = {-squareSize / 2, -squareSize / 2};
        }
      }
    }
  }

  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && dragging) {
    int newCol = static_cast<int>(mousePos.x / squareSize);
    int newRow = static_cast<int>(mousePos.y / squareSize);

    // Only make the move if both the start and end positions are within the
    // board
    if (dragRow >= 0 && dragRow < 8 && dragCol >= 0 && dragCol < 8 &&
        newRow >= 0 && newRow < 8 && newCol >= 0 && newCol < 8) {
      if (currentPosition.makeMove(dragRow, dragCol, newRow, newCol)) {
        // Move was successful - add to move history
        std::string algebraic =
            moveToAlgebraic(dragRow, dragCol, newRow, newCol, draggedPiece);
        // For now, SAN is the same as algebraic; can be improved later
        addMoveToHistory(algebraic, algebraic);

        // Update engine position if enabled
        if (uciEngine->isEnabled()) {
          updateEnginePosition();
        }
      }
    }

    dragging = false;
    draggedPiece = PieceType::EMPTY;
  }
}

void ChessAnalysisProgram::renderGame() {
  BeginDrawing();
  ClearBackground(RAYWHITE);
  renderBoard();
  renderPieces();
  renderEngineAnalysis();
  renderMoveHistory();
  EndDrawing();
}

void ChessAnalysisProgram::run() {
  while (!WindowShouldClose()) {
    updateGame();
    renderGame();
  }
}

// UCI Engine helper methods

void ChessAnalysisProgram::updateEnginePosition() {
  if (uciEngine && uciEngine->isEnabled()) {
    // Convert Move vector to string vector for UCI engine
    std::vector<std::string> moves;
    for (const auto& move : moveHistory) {
      moves.push_back(move.algebraic);
    }
    uciEngine->setPosition(startingFen, moves);
  }
}

std::string ChessAnalysisProgram::moveToAlgebraic(int startRow, int startCol,
                                                  int endRow, int endCol,
                                                  PieceType piece) {
  // Convert to UCI format (e.g., "e2e4")
  char startFile = 'a' + startCol;
  char startRank = '8' - startRow;
  char endFile = 'a' + endCol;
  char endRank = '8' - endRow;

  std::string move;
  move += startFile;
  move += startRank;
  move += endFile;
  move += endRank;

  // Handle pawn promotion (always promote to queen for simplicity)
  if ((piece == PieceType::WHITE_PAWN && endRow == 0) ||
      (piece == PieceType::BLACK_PAWN && endRow == 7)) {
    move += 'q';
  }

  return move;
}

std::string ChessAnalysisProgram::parseEvaluation(const std::string &infoLine) {
  // Parse evaluation from info line
  // Look for "cp" (centipawns) or "mate"
  std::regex cpRegex(R"(cp\s+(-?\d+))");
  std::regex mateRegex(R"(mate\s+(-?\d+))");
  std::smatch match;

  if (std::regex_search(infoLine, match, mateRegex)) {
    int mateIn = std::stoi(match[1]);
    return "Mate in " + std::to_string(std::abs(mateIn));
  } else if (std::regex_search(infoLine, match, cpRegex)) {
    int centipawns = std::stoi(match[1]);
    double pawns = centipawns / 100.0;
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%+.2f", pawns);
    return std::string(buffer);
  }

  return "N/A";
}

std::string ChessAnalysisProgram::parseDepth(const std::string &infoLine) {
  // Parse depth and selective depth
  std::regex depthRegex(R"(depth\s+(\d+))");
  std::regex seldepthRegex(R"(seldepth\s+(\d+))");
  std::smatch match;

  std::string result;
  if (std::regex_search(infoLine, match, depthRegex)) {
    result = "D" + match[1].str();
  }

  if (std::regex_search(infoLine, match, seldepthRegex)) {
    if (!result.empty())
      result += "/";
    result += "SD" + match[1].str();
  }

  return result.empty() ? "N/A" : result;
}

std::string ChessAnalysisProgram::parsePV(const std::string &infoLine) {
  // Parse principal variation
  size_t pvPos = infoLine.find(" pv ");
  if (pvPos != std::string::npos) {
    std::string pv = infoLine.substr(pvPos + 4);
    // Limit to first 5 moves for display
    std::istringstream iss(pv);
    std::string move;
    std::string result;
    int count = 0;
    while (iss >> move && count < 5) {
      if (count > 0)
        result += " ";
      result += move;
      count++;
    }
    return result;
  }
  return "N/A";
}

void ChessAnalysisProgram::renderEngineAnalysis() {
  if (!uciEngine || !uciEngine->isEnabled()) {
    // Draw disabled indicator
    DrawText("Engine: OFF (Press X to enable)", 1100, 20, 20, DARKGRAY);
    return;
  }

  // Draw enabled indicator
  DrawText("Engine: ON (Press X to disable)", 1100, 20, 20, GREEN);

  // Check if we have analysis results
  if (!currentAnalysis.hasResult) {
    DrawText("Analyzing...", 1100, 50, 18, GRAY);
    return;
  }

  // Display analysis lines
  int yPos = 60;
  int lineHeight = 80;

  for (size_t i = 0; i < currentAnalysis.lines.size() && i < 4; i++) {
    const auto &line = currentAnalysis.lines[i];

    // Parse the info line
    std::string eval = parseEvaluation(line.text);
    std::string depth = parseDepth(line.text);
    std::string pv = parsePV(line.text);

    // Draw line number
    DrawText(TextFormat("Line %d:", line.multipv), 1100, yPos, 18, DARKBLUE);

    // Draw evaluation
    Color evalColor = (eval[0] == '+')   ? DARKGREEN
                      : (eval[0] == '-') ? RED
                                         : GRAY;
    DrawText(TextFormat("Eval: %s", eval.c_str()), 1100, yPos + 20, 16,
             evalColor);

    // Draw depth
    DrawText(TextFormat("Depth: %s", depth.c_str()), 1100, yPos + 40, 14,
             DARKGRAY);

    // Draw PV
    DrawText(TextFormat("PV: %s", pv.c_str()), 1100, yPos + 58, 14, BLACK);

    yPos += lineHeight;
  }
}

void ChessAnalysisProgram::renderMoveHistory() {
  // Draw move history on the right side of the screen
  int screenWidth = GetScreenWidth();
  int xPos = screenWidth - 375;
  int yPos = 20;
  int lineHeight = 22;
  int maxMovesToDisplay = 20;
  
  // Draw title
  DrawText("Move History:", xPos, yPos, 18, DARKBLUE);
  yPos += 28;
  
  // Calculate starting index (show last maxMovesToDisplay moves)
  int startIdx = static_cast<int>(moveHistory.size()) > maxMovesToDisplay 
                 ? moveHistory.size() - maxMovesToDisplay 
                 : 0;
  
  // Draw move pairs (white and black moves side-by-side)
  for (size_t i = startIdx; i < moveHistory.size(); i += 2) {
    int moveNumber = (i / 2) + 1;
    
    // Draw move number
    DrawText(TextFormat("%2d.", moveNumber), xPos, yPos, 16, DARKGRAY);
    int textXPos = xPos + 35;
    
    // Draw white's move
    if (i < moveHistory.size()) {
      DrawText(moveHistory[i].algebraic.c_str(), textXPos, yPos, 16, BLACK);
      textXPos += 70;
    }
    
    // Draw black's move on the same line if it exists
    if (i + 1 < moveHistory.size()) {
      DrawText(moveHistory[i + 1].algebraic.c_str(), textXPos, yPos, 16, BLACK);
    }
    
    yPos += lineHeight;
  }
  
  // Draw move counter at bottom
  DrawText(TextFormat("Total moves: %d", (int)moveHistory.size()), xPos, yPos + 10, 14, DARKGRAY);
  
  // Draw keyboard hints
  int hintYPos = GetScreenHeight() - 80;
  DrawText("Controls:", xPos, hintYPos, 14, DARKGRAY);
  DrawText("R - Reset  |  U/Left Arrow - Undo  |  X - Toggle Engine", xPos, hintYPos + 20, 12, GRAY);
}

void ChessAnalysisProgram::addMoveToHistory(const std::string &algebraic, const std::string &san) {
  // Determine move number and whose turn it was
  int moveCount = moveHistory.size();
  int moveNumber = (moveCount / 2) + 1;
  bool wasWhiteMove = (moveCount % 2 == 0);  // White moves on even indices
  
  Move newMove;
  newMove.algebraic = algebraic;
  newMove.sanNotation = san;
  newMove.moveNumber = moveNumber;
  newMove.isWhiteMove = wasWhiteMove;
  
  moveHistory.push_back(newMove);
  
  TraceLog(LOG_INFO, "Move added to history: %s (Move %d, %s)",
           algebraic.c_str(), moveNumber, wasWhiteMove ? "White" : "Black");
}