#include "chess_analysis_program.h"
#include <raylib.h>
#include <regex>
#include <sstream>
#include <fstream>

ChessAnalysisProgram::ChessAnalysisProgram() {
  // Initialization
  const int screenWidth = 1920;
  const int screenHeight = 1080;

  // Set fullscreen flag before initializing window
  SetConfigFlags(FLAG_FULLSCREEN_MODE);
  InitWindow(screenWidth, screenHeight, "Chess Analysis Program");

  SetTargetFPS(120); // Set the desired frame rate

  // Load textures after window creation (some platforms require an OpenGL
  // context)
  loadAllTextures();

  // Initialize UCI engine (disabled by default)
  // Update this path to point to your Stockfish executable
  uciEngine = std::make_unique<UCIEngine>("src/stockfish/stockfish.exe");

  // Load starting FEN from file, or use standard starting position as fallback
  std::ifstream fenFile("src/initial_position.fen");
  if (fenFile.is_open()) {
    std::string fenString;
    if (std::getline(fenFile, fenString)) {
      startingFen = fenString;
      TraceLog(LOG_INFO, "Loaded starting FEN from initial_position.fen: %s", startingFen.c_str());
    }
    fenFile.close();
  } else {
    // Use standard starting position as fallback
    startingFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    TraceLog(LOG_WARNING, "initial_position.fen not found, using standard starting position");
  }
  
  // Initialize currentBaseFen to the starting FEN (L key state)
  currentBaseFen = startingFen;
  moveHistory.clear();
  // Initialize position history with the starting position
  positionHistory.clear();
  positionHistory[generatePositionKey()] = 1;
}

ChessAnalysisProgram::~ChessAnalysisProgram() {
  unloadAllTextures();
  CloseWindow(); // Close window and OpenGL context
}

void ChessAnalysisProgram::loadAllTextures() {
  // Load assets
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

void ChessAnalysisProgram::renderBoard() {
  // Calculate layout with borders for coordinates
  int screenHeight = GetScreenHeight();
  int screenWidth = GetScreenWidth();
  int labelSize = 40;  // Size of coordinate label strips
  int padding = 10;    // Padding around board
  
  // Calculate board scale to fit with labels (max 70% of screen height for board itself)
  float maxBoardHeight = screenHeight * 0.7f;
  float boardScale = maxBoardHeight / (float)boardTexture.height;
  
  // Calculate board dimensions
  float boardWidth = boardTexture.width * boardScale;
  float boardHeight = boardTexture.height * boardScale;
  
  // Position board on left side with padding
  float totalWidth = boardWidth + labelSize + (padding * 2);
  float totalHeight = boardHeight + labelSize + (padding * 2);
  float startX = padding * 2;  // Left-aligned with small padding
  float startY = (screenHeight - totalHeight) / 2.0f;  // Vertically centered
  
  // Draw top label strip (files: a-h)
  DrawRectangle(startX + labelSize, startY, (int)boardWidth, labelSize, LIGHTGRAY);
  for (int i = 0; i < 8; i++) {
    // File labels: when flipped, show h->a left-to-right
    char fileChar = flipBoard ? ('h' - i) : ('a' + i);
    float labelX = startX + labelSize + (boardWidth / 8.0f) * i + (boardWidth / 16.0f) - 5;
    DrawText(TextFormat("%c", fileChar), (int)labelX, (int)(startY + 12), 16, BLACK);
  }
  
  // Draw left label strip (ranks: 8-1)
  DrawRectangle(startX, startY + labelSize, labelSize, (int)boardHeight, LIGHTGRAY);
  for (int i = 0; i < 8; i++) {
    // Rank labels: when flipped, show 1..8 top-to-bottom
    int rank = flipBoard ? (i + 1) : (8 - i);
    float labelY = startY + labelSize + (boardHeight / 8.0f) * i + (boardHeight / 16.0f) - 8;
    DrawText(TextFormat("%d", rank), (int)(startX + 12), (int)labelY, 16, BLACK);
  }
  
  // Draw the board itself
  // Draw the board texture (no change). Pieces/coordinates will be mapped visually.
  DrawTextureEx(this->boardTexture, {startX + labelSize, startY + labelSize}, 0.0f, boardScale, WHITE);
}

void ChessAnalysisProgram::renderPieces() {
  // Match board positioning from renderBoard()
  int screenHeight = GetScreenHeight();
  int screenWidth = GetScreenWidth();
  int labelSize = 40;
  int padding = 10;
  
  float maxBoardHeight = screenHeight * 0.7f;
  float boardScale = maxBoardHeight / (float)boardTexture.height;
  
  float boardWidth = boardTexture.width * boardScale;
  float boardHeight = boardTexture.height * boardScale;
  
  float totalWidth = boardWidth + labelSize + (padding * 2);
  float totalHeight = boardHeight + labelSize + (padding * 2);
  float startX = padding * 2;
  float startY = (screenHeight - totalHeight) / 2.0f;
  
  float boardStartX = startX + labelSize;
  float boardStartY = startY + labelSize;
  float squareSize = boardWidth / 8.0f;
  float pieceScale = boardScale * 1.0f;
  float pieceOffset = (squareSize - (pieceScale * boardTexture.width / 10.5f)) / 2.0f;

  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      PieceType piece = currentPosition.getPieceAt(row, col);
      if (piece == PieceType::EMPTY)
        continue;

      // Skip drawing the dragged piece (so we draw it separately)
      if (dragging && row == dragRow && col == dragCol)
        continue;

      // Map logical board coordinates to visual coordinates when flipped
      int visRow = flipBoard ? (7 - row) : row;
      int visCol = flipBoard ? (7 - col) : col;

      Texture2D tex = getTextureForPiece(piece);
      Vector2 pos = {boardStartX + visCol * squareSize + pieceOffset,
                     boardStartY + visRow * squareSize + pieceOffset};
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
  // Match board positioning from renderBoard()
  int screenHeight = GetScreenHeight();
  int screenWidth = GetScreenWidth();
  int labelSize = 40;
  int padding = 10;
  
  float maxBoardHeight = screenHeight * 0.7f;
  float boardScale = maxBoardHeight / (float)boardTexture.height;
  
  float boardWidth = boardTexture.width * boardScale;
  float boardHeight = boardTexture.height * boardScale;
  
  float totalWidth = boardWidth + labelSize + (padding * 2);
  float totalHeight = boardHeight + labelSize + (padding * 2);
  float startX = padding * 2;
  float startY = (screenHeight - totalHeight) / 2.0f;
  
  float boardStartX = startX + labelSize;
  float boardStartY = startY + labelSize;
  float squareSize = boardWidth / 8.0f;
  
  Vector2 mousePos = GetMousePosition();

  // Calculate board-relative position
  float relMouseX = mousePos.x - boardStartX;
  float relMouseY = mousePos.y - boardStartY;
  int col = static_cast<int>(relMouseX / squareSize);
  int row = static_cast<int>(relMouseY / squareSize);

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

  // Flip board visuals
  if (IsKeyPressed(KEY_F)) {
    flipBoard = !flipBoard;
    TraceLog(LOG_INFO, "Board flip toggled: %s", flipBoard ? "ON" : "OFF");
  }

  if (IsKeyPressed(KEY_R)) {
    // Reset game to standard chess starting position
    currentBaseFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    currentPosition.initializeBoard();
    currentPosition.setWhiteTurn(true);
    moveHistory.clear();
    positionHistory.clear();
    positionHistory[generatePositionKey()] = 1;  // Initial position
    lastEnginePositionFen = "";  // Force engine update on next frame
    TraceLog(LOG_INFO, "Game reset to standard chess starting position");
    // Update engine position if enabled
    if (uciEngine->isEnabled()) {
      updateEnginePosition();
    }
  }

  if (IsKeyPressed(KEY_L)) {
    // Reset game to starting position from FEN
    currentBaseFen = startingFen;  // Use the loaded FEN as base
    currentPosition.loadFromFEN(startingFen.c_str());
    moveHistory.clear();
    positionHistory.clear();
    positionHistory[generatePositionKey()] = 1;  // Initial position
    lastEnginePositionFen = "";  // Force engine update on next frame
    TraceLog(LOG_INFO, "Game reset to FEN starting position");
    // Update engine position if enabled
    if (uciEngine->isEnabled()) {
      updateEnginePosition();
    }
  }

  if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_U)) {
    if (!moveHistory.empty()) {
      // Undo last move
      moveHistory.pop_back();
      // Reset to the current base FEN (standard or loaded) before replaying
      if (currentBaseFen == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1") {
        currentPosition.initializeBoard();
        currentPosition.setWhiteTurn(true);
      } else {
        currentPosition.loadFromFEN(currentBaseFen.c_str());
      }
      
      // Rebuild position history
      positionHistory.clear();
      positionHistory[generatePositionKey()] = 1;
      
      // Replay all moves except the last one
      for (const auto& move : moveHistory) {
        // Parse and replay move from algebraic notation
        int startCol = move.algebraic[0] - 'a';
        int startRow = 8 - (move.algebraic[1] - '0');
        int endCol = move.algebraic[2] - 'a';
        int endRow = 8 - (move.algebraic[3] - '0');
        currentPosition.makeMove(startRow, startCol, endRow, endCol);
        // Track position for threefold repetition detection
        {
          std::string key = generatePositionKey();
          positionHistory[key]++;
          if (positionHistory[key] >= 3) {
            currentPosition.declareDraw("Threefold repetition");
          }
        }
      }
      
      lastEnginePositionFen = "";  // Force engine update on next frame
      
      // Force engine to refresh for new position and update if enabled
      lastEnginePositionFen = "";  // Force engine update on next frame
      if (uciEngine->isEnabled()) {
        updateEnginePosition();
      }
      
      TraceLog(LOG_INFO, "Move undone");
    }
  }

  // Poll engine analysis if enabled
  if (uciEngine->isEnabled()) {
    // Update engine position every frame to ensure it's analyzing current position
    std::vector<std::string> moves;
    for (const auto& move : moveHistory) {
      moves.push_back(move.algebraic);
    }
    std::string currentFen = startingFen + "|" + std::to_string(moveHistory.size());
    
    // Detect position change and reset analysis
    if (currentFen != lastEnginePositionFen) {
      lastEnginePositionFen = currentFen;
      currentAnalysis = EngineAnalysis();  // Clear old analysis
      updateEnginePosition();  // Set new position
      TraceLog(LOG_INFO, "Engine position updated - analyzing new position");
    }
    
    currentAnalysis = uciEngine->pollAnalysis();
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (row >= 0 && row < 8 && col >= 0 && col < 8) { // Make sure we're clicking within the board
      // Map visual coords to logical when flipped (so clicks correspond to visual board)
      int logicalCol = flipBoard ? (7 - col) : col;
      int logicalRow = flipBoard ? (7 - row) : row;
      PieceType piece = currentPosition.getPieceAt(logicalRow, logicalCol);
      if (piece != PieceType::EMPTY) {
        bool isWhitePiece =
            (piece == PieceType::WHITE_PAWN ||
             piece == PieceType::WHITE_KNIGHT ||
             piece == PieceType::WHITE_BISHOP ||
             piece == PieceType::WHITE_ROOK ||
             piece == PieceType::WHITE_QUEEN || piece == PieceType::WHITE_KING);
        // Only allow dragging pieces that belong to the current player
        if ((currentPosition.isWhiteTurn() && isWhitePiece) || (!currentPosition.isWhiteTurn() && !isWhitePiece)) {
          dragging = true;
          // Store logical coords for the drag
          dragRow = logicalRow;
          dragCol = logicalCol;
          draggedPiece = piece;
          dragOffset = {-squareSize / 2, -squareSize / 2};
        }
      }
    }
  }

  if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && dragging) {
    float newRelMouseX = mousePos.x - boardStartX;
    float newRelMouseY = mousePos.y - boardStartY;
    int newCol = static_cast<int>(newRelMouseX / squareSize);
    int newRow = static_cast<int>(newRelMouseY / squareSize);

    // Map visual release coords to logical when flipped
    int logicalNewCol = flipBoard ? (7 - newCol) : newCol;
    int logicalNewRow = flipBoard ? (7 - newRow) : newRow;

    // Only make the move if both the start and end positions are within the
    // board and threefold repetition hasn't been detected
    if (!isThreefoldRepetition() && dragRow >= 0 && dragRow < 8 && dragCol >= 0 && dragCol < 8 && logicalNewRow >= 0 && logicalNewRow < 8 && logicalNewCol >= 0 && logicalNewCol < 8) {
      if (currentPosition.makeMove(dragRow, dragCol, logicalNewRow, logicalNewCol)) {
        // Move was successful - add to move history
        std::string algebraic = moveToAlgebraic(dragRow, dragCol, logicalNewRow, logicalNewCol, draggedPiece);
        addMoveToHistory(algebraic, algebraic);

        // Track position for threefold repetition detection
        {
          std::string key = generatePositionKey();
          positionHistory[key]++;
          if (positionHistory[key] >= 3) {
            currentPosition.declareDraw("Threefold repetition");
          }
        }

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
  ClearBackground({40, 44, 52, 255});  // Dark gray background
  renderBoard();
  renderPieces();
  renderGameStatus();
  renderEngineAnalysis();
  renderMoveHistory();
  renderControls();
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
    // Use currentBaseFen which changes based on R/L key press
    uciEngine->setPosition(currentBaseFen, moves);
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
  int screenWidth = GetScreenWidth();
  int screenHeight = GetScreenHeight();
  int boxWidth = 300;
  int boxHeight = 380;
  int xPos = screenWidth - boxWidth - 20;
  int yPos = 20;
  int padding = 10;
  
  // Draw box background
  DrawRectangle(xPos, yPos, boxWidth, boxHeight, {50, 55, 65, 255});  // Dark slate blue
  // Draw box border
  DrawRectangleLines(xPos, yPos, boxWidth, boxHeight, {100, 200, 255, 255});  // Light blue border
  
  // Draw title
  DrawText("UCI Engine Analysis", xPos + padding, yPos + padding, 14, {100, 200, 255, 255});
  
  int contentYPos = yPos + 28;
  
  // Draw mode indicator
  bool isStandardFen = (currentBaseFen == "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
  const char* modeText = isStandardFen ? "Mode: Standard (R)" : "Mode: FEN (L)";
  Color modeColor = isStandardFen ? (Color){200, 200, 200, 255} : (Color){200, 200, 200, 255};
  DrawText(modeText, xPos + padding, contentYPos, 10, modeColor);
  
  contentYPos += 15;
  
  if (!uciEngine || !uciEngine->isEnabled()) {
    // Draw disabled indicator
    DrawText("Engine: OFF", xPos + padding, contentYPos, 14, {180, 180, 180, 255});
    DrawText("(Press X to enable)", xPos + padding, contentYPos + 20, 12, {150, 150, 150, 255});
    return;
  }

  // Draw enabled indicator
  DrawText("Engine: ON", xPos + padding, contentYPos, 14, {100, 255, 150, 255});

  // Check if we have analysis results
  if (!currentAnalysis.hasResult) {
    DrawText("Analyzing...", xPos + padding, contentYPos + 25, 12, {150, 150, 150, 255});
    return;
  }

  // Display analysis lines
  int yPos_lines = contentYPos + 25;
  int lineHeight = 85;

  for (size_t i = 0; i < currentAnalysis.lines.size() && i < 4; i++) {
    const auto &line = currentAnalysis.lines[i];

    // Parse the info line
    std::string eval = parseEvaluation(line.text);
    std::string depth = parseDepth(line.text);
    std::string pv = parsePV(line.text);

    // Draw line number
    DrawText(TextFormat("Line %d:", line.multipv), xPos + padding, yPos_lines, 12, {100, 200, 255, 255});

    // Draw evaluation
    Color evalColor = (eval[0] == '+')   ? (Color){100, 255, 150, 255}
                      : (eval[0] == '-') ? (Color){255, 100, 100, 255}
                                         : (Color){200, 200, 200, 255};
    DrawText(TextFormat("Eval: %s", eval.c_str()), xPos + padding, yPos_lines + 18, 11, evalColor);

    // Draw depth
    DrawText(TextFormat("Depth: %s", depth.c_str()), xPos + padding, yPos_lines + 33, 10, {180, 180, 180, 255});

    // Draw PV (truncated)
    std::string pvDisplay = pv.length() > 30 ? pv.substr(0, 27) + "..." : pv;
    DrawText(TextFormat("PV: %s", pvDisplay.c_str()), xPos + padding, yPos_lines + 46, 10, {220, 220, 220, 255});

    yPos_lines += lineHeight;
  }
}

void ChessAnalysisProgram::renderMoveHistory() {
  // Draw move history on the right side of the screen
  int screenWidth = GetScreenWidth();
  int screenHeight = GetScreenHeight();
  int boxWidth = 300;
  int boxHeight = 500;
  int xPos = screenWidth - boxWidth - 20;
  int yPos = 420;
  int padding = 10;
  int lineHeight = 20;
  int maxMovesToDisplay = 20;
  
  // Draw box background
  DrawRectangle(xPos, yPos, boxWidth, boxHeight, {50, 65, 55, 255});  // Dark sage green
  // Draw box border
  DrawRectangleLines(xPos, yPos, boxWidth, boxHeight, {120, 220, 140, 255});  // Light green border
  
  // Draw title
  DrawText("Move History", xPos + padding, yPos + padding, 14, {120, 220, 140, 255});
  int contentYPos = yPos + 28;
  
  // Calculate starting index (show last maxMovesToDisplay moves)
  int startIdx = static_cast<int>(moveHistory.size()) > maxMovesToDisplay 
                 ? moveHistory.size() - maxMovesToDisplay 
                 : 0;
  
  // Draw moves one per line with move numbers
  int displayYPos = contentYPos;
  for (size_t i = startIdx; i < moveHistory.size(); i++) {
    int moveNumber = (i / 2) + 1;
    bool isWhiteMove = (i % 2 == 0);
    
    // Draw move number and move notation
    std::string moveLabel = TextFormat("%d. %s", moveNumber, moveHistory[i].algebraic.c_str());
    
    // Color: white moves in light gray, black moves in darker gray
    Color moveColor = isWhiteMove ? (Color){255, 255, 255, 255} : (Color){0, 0, 0, 255};
    
    DrawText(moveLabel.c_str(), xPos + padding, displayYPos, 12, moveColor);
    
    displayYPos += lineHeight;
  }
  
  // Draw move counter
  DrawText(TextFormat("Total: %d", (int)moveHistory.size()), xPos + padding, yPos + boxHeight - 30, 12, {180, 180, 180, 255});
}

void ChessAnalysisProgram::renderControls() {
  int screenWidth = GetScreenWidth();
  int screenHeight = GetScreenHeight();
  int boxWidth = 350;
  int boxHeight = 80;
  int xPos = screenWidth - boxWidth - 20;
  int yPos = screenHeight - boxHeight - 20;
  int padding = 10;
  
  // Draw box background
  DrawRectangle(xPos, yPos, boxWidth, boxHeight, {65, 55, 50, 255});  // Dark burnt orange
  // Draw box border
  DrawRectangleLines(xPos, yPos, boxWidth, boxHeight, {255, 180, 100, 255});  // Light orange border
  
  // Draw title
  DrawText("Controls", xPos + padding, yPos + padding, 14, {255, 180, 100, 255});
  
  // Draw threefold repetition draw status if detected
  // Draw control descriptions
  int controlStartY = yPos + 28;
  DrawText("R - Reset (Standard)", xPos + padding, controlStartY, 10, {230, 230, 230, 255});
  DrawText("L - Reset (FEN)", xPos + padding, controlStartY + 12, 10, {230, 230, 230, 255});
  DrawText("U / Left - Undo", xPos + padding, controlStartY + 24, 10, {230, 230, 230, 255});
  DrawText("X - Engine", xPos + padding, controlStartY + 36, 10, {230, 230, 230, 255});
  DrawText("F - Flip Board", xPos + 200, controlStartY, 10, {230, 230, 230, 255});
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

void ChessAnalysisProgram::renderGameStatus() {
  int screenWidth = GetScreenWidth();
  int screenHeight = GetScreenHeight();
  
  // Box dimensions and position (larger box shifted right)
  int boxWidth = 450;
  int boxHeight = 650;
  int boardRightX = 100 + 500;  // Board starts at 100, 500 pixels wide
  int xPos = boardRightX + 350;  // Shifted more to the right
  int yPos = (screenHeight - boxHeight) / 2;
  int padding = 15;
  int pieceSize = 28;
  
  // Draw box background
  DrawRectangle(xPos, yPos, boxWidth, boxHeight, {60, 50, 45, 255});  // Dark brown
  DrawRectangleLines(xPos, yPos, boxWidth, boxHeight, {200, 150, 100, 255});  // Brown border
  
  // Game Status
  DrawText("Game Status", xPos + padding, yPos + padding, 12, {220, 190, 160, 255});
  
  // Determine game status string
  std::string statusStr = "Ongoing";
  Color statusColor = {100, 200, 100, 255};  // Green
  
  GameStatus gameStatus = currentPosition.getGameStatus();
  
  // Prefer explicit game status from ChessGame over direct repetition checks
  if (gameStatus == GameStatus::CHECKMATE) {
    statusStr = "Checkmate";
    statusColor = {255, 100, 100, 255};  // Red
  } else if (gameStatus == GameStatus::STALEMATE || gameStatus == GameStatus::DRAW) {
    statusStr = "Draw";
    statusColor = {150, 150, 150, 255};  // Gray
  } else if (gameStatus == GameStatus::CHECK) {
    // If CHECK status, verify it's not actually checkmate
    // (checkmate is when the player whose turn it is is in check AND has no legal moves)
    bool currentPlayerIsCheckmated = currentPosition.isCheckmatePublic(currentPosition.isWhiteTurn());
    if (currentPlayerIsCheckmated) {
      statusStr = "Checkmate";
      statusColor = {255, 100, 100, 255};
      gameStatus = GameStatus::CHECKMATE;
    } else {
      statusStr = "Check!";
      statusColor = {255, 200, 50, 255};  // Orange
    }
  } else if (isThreefoldRepetition()) {
    // Fallback display if repetition detected but game status not yet updated
    statusStr = "Draw: 3-fold Rep.";
    statusColor = {150, 150, 150, 255};  // Gray
  }
  
  DrawText(statusStr.c_str(), xPos + padding, yPos + 35, 16, statusColor);
  
  // Draw current player only when the game is ongoing or in check (but not checkmate)
  if (statusStr == "Checkmate") {
    // Show the winner on checkmate
    std::string winnerStr = currentPosition.isWhiteTurn() ? "Black Wins" : "White Wins";
    DrawText(winnerStr.c_str(), xPos + padding, yPos + 60, 13, {255, 120, 120, 255});
  } else if (gameStatus == GameStatus::ONGOING || gameStatus == GameStatus::CHECK) {
    std::string playerStr = currentPosition.isWhiteTurn() ? "White to move" : "Black to move";
    Color playerColor = currentPosition.isWhiteTurn() ? (Color){255, 255, 255, 255} : (Color){100, 100, 100, 255};
    DrawText(playerStr.c_str(), xPos + padding, yPos + 60, 13, playerColor);
  }
  
  // Captured Pieces
  int captureYStart = yPos + 140;
  DrawText("Captured Pieces", xPos + padding, captureYStart, 13, {220, 190, 160, 255});
  
  // White captured pieces (captured by black)
  DrawText("By Black:", xPos + padding, captureYStart + 25, 11, {200, 200, 200, 255});
  int whiteCaptX = xPos + padding;
  int whiteCaptY = captureYStart + 45;
  
  for (size_t i = 0; i < currentPosition.getWhiteCapturedPieces().size(); i++) {
    PieceType piece = currentPosition.getWhiteCapturedPieces()[i];
    Texture2D tex = getTextureForPiece(piece);
    DrawTextureEx(tex, {(float)(whiteCaptX + (i % 12) * 32), (float)whiteCaptY}, 0, 
                  (float)pieceSize / tex.width, WHITE);
    if ((i + 1) % 12 == 0) {
      whiteCaptY += 35;
    }
  }
  
  // Black captured pieces (captured by white)
  int blackCaptY = whiteCaptY + (currentPosition.getWhiteCapturedPieces().size() > 0 ? 40 : 20);
  DrawText("By White:", xPos + padding, blackCaptY, 11, {200, 200, 200, 255});
  int blackCaptX = xPos + padding;
  int blackCaptStartY = blackCaptY + 15;
  
  for (size_t i = 0; i < currentPosition.getBlackCapturedPieces().size(); i++) {
    PieceType piece = currentPosition.getBlackCapturedPieces()[i];
    Texture2D tex = getTextureForPiece(piece);
    DrawTextureEx(tex, {(float)(blackCaptX + (i % 12) * 32), (float)blackCaptStartY}, 0, 
                  (float)pieceSize / tex.width, WHITE);
    if ((i + 1) % 12 == 0) {
      blackCaptStartY += 35;
    }
  }
}

bool ChessAnalysisProgram::isThreefoldRepetition() const {
  std::string key = generatePositionKey();
  auto it = positionHistory.find(key);
  if (it != positionHistory.end()) {
    return it->second >= 3;
  }
  return false;
}

// Build a normalized position key for repetition detection.
// Keep only the first 4 fields of FEN: board, side to move, castling, en-passant.
std::string ChessAnalysisProgram::generatePositionKey() const {
  std::string fen = currentPosition.generateFEN();
  std::istringstream iss(fen);
  std::string part;
  std::string key;
  int fields = 0;
  while (iss >> part && fields < 4) {
    if (fields > 0) key += ' ';
    key += part;
    fields++;
  }
  return key;
}