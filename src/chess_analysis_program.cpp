#include <raylib.h>
#include "chess_analysis_program.h"

ChessAnalysisProgram::ChessAnalysisProgram() {
    // Initialization
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "Chess Analysis Program");

    SetTargetFPS(120); // Set the desired frame rate

    // Load textures after window creation (some platforms require an OpenGL context)
    loadAllTextures();
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
    }

    // Note: filenames are assumed to be present in src/assets/chess_pieces/ or images/chess_pieces/ (download and place them accordingly)
    this->whiteKnightTexture = LoadTexture("src/images/chess_pieces/wn.png");
    if (!this->whiteKnightTexture.id) {
        TraceLog(LOG_ERROR, "Failed to load white knight texture!");
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
    if (boardTexture.id) UnloadTexture(boardTexture);
    if (whiteKnightTexture.id) UnloadTexture(whiteKnightTexture);
    if (whiteBishopTexture.id) UnloadTexture(whiteBishopTexture);
    if (whiteRookTexture.id) UnloadTexture(whiteRookTexture);
    if (whiteKingTexture.id) UnloadTexture(whiteKingTexture);
    if (whiteQueenTexture.id) UnloadTexture(whiteQueenTexture);
    if (whitePawnTexture.id) UnloadTexture(whitePawnTexture);
    if (blackKnightTexture.id) UnloadTexture(blackKnightTexture);
    if (blackBishopTexture.id) UnloadTexture(blackBishopTexture);
    if (blackRookTexture.id) UnloadTexture(blackRookTexture);
    if (blackKingTexture.id) UnloadTexture(blackKingTexture);
    if (blackQueenTexture.id) UnloadTexture(blackQueenTexture);
    if (blackPawnTexture.id) UnloadTexture(blackPawnTexture);
}

void ChessAnalysisProgram::renderUI(){
    // Render user interface elements here
}

void ChessAnalysisProgram::renderBoard(){
    DrawTextureEx(this->boardTexture, {0, 0}, 0.0f, 0.65f, WHITE);
}

void ChessAnalysisProgram::renderPieces() {
    float squareSize = boardTexture.width * 0.676f / 8.0f;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            PieceType piece = currentPosition.getPieceAt(row, col);
            if (piece == PieceType::EMPTY) continue;

            // Skip drawing the dragged piece (so we draw it separately)
            if (dragging && row == dragRow && col == dragCol)
                continue;

            Texture2D tex = getTextureForPiece(piece);
            Vector2 pos = { col * squareSize, row * squareSize };
            DrawTextureEx(tex, pos, 0.0f, 0.65f, WHITE);
        }
    }

    // Draw dragged piece under cursor
    if (dragging && draggedPiece != PieceType::EMPTY) {
        Texture2D tex = getTextureForPiece(draggedPiece);
        Vector2 pos = GetMousePosition();
        pos.x += dragOffset.x;
        pos.y += dragOffset.y;
        DrawTextureEx(tex, pos, 0.0f, 0.65f, WHITE);
    }
}

Texture2D ChessAnalysisProgram::getTextureForPiece(PieceType piece) {
    switch (piece) {
        case PieceType::WHITE_PAWN:   return whitePawnTexture;
        case PieceType::WHITE_KNIGHT: return whiteKnightTexture;
        case PieceType::WHITE_BISHOP: return whiteBishopTexture;
        case PieceType::WHITE_ROOK:   return whiteRookTexture;
        case PieceType::WHITE_QUEEN:  return whiteQueenTexture;
        case PieceType::WHITE_KING:   return whiteKingTexture;
        case PieceType::BLACK_PAWN:   return blackPawnTexture;
        case PieceType::BLACK_KNIGHT: return blackKnightTexture;
        case PieceType::BLACK_BISHOP: return blackBishopTexture;
        case PieceType::BLACK_ROOK:   return blackRookTexture;
        case PieceType::BLACK_QUEEN:  return blackQueenTexture;
        case PieceType::BLACK_KING:   return blackKingTexture;
        default: return Texture2D{};
    }
}

void ChessAnalysisProgram::updateGame() {
    float squareSize = boardTexture.width * 0.65f / 8.0f;
    Vector2 mousePos = GetMousePosition();
    int col = static_cast<int>(mousePos.x / squareSize);
    int row = static_cast<int>(mousePos.y / squareSize);

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        PieceType piece = currentPosition.getPieceAt(row, col);
        if (piece != PieceType::EMPTY) {
            dragging = true;
            dragRow = row;
            dragCol = col;
            draggedPiece = piece;
            dragOffset = { -squareSize / 2, -squareSize / 2 };
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && dragging) {
        int newCol = static_cast<int>(mousePos.x / squareSize);
        int newRow = static_cast<int>(mousePos.y / squareSize);
        currentPosition.makeMove(dragRow, dragCol, newRow, newCol);
        dragging = false;
        draggedPiece = PieceType::EMPTY;
    }
}

void ChessAnalysisProgram::renderGame() {
    BeginDrawing();
    ClearBackground(RAYWHITE);
    renderBoard();
    renderPieces();
    EndDrawing();
}

void ChessAnalysisProgram::run() {
    while (!WindowShouldClose()) {
        updateGame();
        renderGame();
    }
}