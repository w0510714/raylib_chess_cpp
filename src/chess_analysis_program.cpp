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

    currentPosition = ChessGame();
}
void ChessAnalysisProgram::loadAllTextures() {
    // Load assets
    this->boardTexture = LoadTexture("src/assets/board.png");
    if (!this->boardTexture.id) {
        TraceLog(LOG_ERROR, "Failed to load board texture!");
    }

    // Note: filenames are assumed to be present in src/assets/chess_pieces/
    this->whiteKnightTexture = LoadTexture("src/assets/chess_pieces/wk.png");
    if (!this->whiteKnightTexture.id) {
        TraceLog(LOG_ERROR, "Failed to load white knight texture!");
    }

    this->whiteBishopTexture = LoadTexture("src/assets/chess_pieces/wb.png");
    if (!this->whiteBishopTexture.id) {
        TraceLog(LOG_ERROR, "Failed to load white bishop texture!");
    }

    this->whiteRookTexture = LoadTexture("src/assets/chess_pieces/wr.png");
    if (!this->whiteRookTexture.id) {
        TraceLog(LOG_ERROR, "Failed to load white rook texture!");
    }

    this->whiteKingTexture = LoadTexture("src/assets/chess_pieces/wk.png");
    if (!this->whiteKingTexture.id) {
        TraceLog(LOG_ERROR, "Failed to load white king texture!");
    }

    this->whiteQueenTexture = LoadTexture("src/assets/chess_pieces/wq.png");
    if (!this->whiteQueenTexture.id) {
        TraceLog(LOG_ERROR, "Failed to load white queen texture!");
    }

    this->whitePawnTexture = LoadTexture("src/assets/chess_pieces/wp.png");
    if (!this->whitePawnTexture.id) {
        TraceLog(LOG_ERROR, "Failed to load white pawn texture!");
    }

    this->blackKnightTexture = LoadTexture("src/assets/chess_pieces/bk.png");
    if (!this->blackKnightTexture.id) {
        TraceLog(LOG_ERROR, "Failed to load black knight texture!");
    }

    this->blackBishopTexture = LoadTexture("src/assets/chess_pieces/bb.png");
    if (!this->blackBishopTexture.id) {
        TraceLog(LOG_ERROR, "Failed to load black bishop texture!");
    }

    this->blackRookTexture = LoadTexture("src/assets/chess_pieces/br.png");
    if (!this->blackRookTexture.id) {
        TraceLog(LOG_ERROR, "Failed to load black rook texture!");
    }

    this->blackKingTexture = LoadTexture("src/assets/chess_pieces/bk.png");
    if (!this->blackKingTexture.id) {
        TraceLog(LOG_ERROR, "Failed to load black king texture!");
    }

    this->blackQueenTexture = LoadTexture("src/assets/chess_pieces/bq.png");
    if (!this->blackQueenTexture.id) {
        TraceLog(LOG_ERROR, "Failed to load black queen texture!");
    }

    this->blackPawnTexture = LoadTexture("src/assets/chess_pieces/bp.png");
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

ChessAnalysisProgram::~ChessAnalysisProgram() {
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

    CloseWindow(); // Close window and OpenGL context
}

void ChessAnalysisProgram::renderUI(){
    // Render user interface elements here
}

void ChessAnalysisProgram::renderBoard(){
    DrawTextureEx(this->boardTexture, {0, 0}, 0.0f, 0.65f, WHITE);
}

Texture2D ChessAnalysisProgram::getTextureForPiece(PieceType Place){
    switch (Place) {
        case PieceType::WHITE_KNIGHT:
            return whiteKnightTexture;
        case PieceType::WHITE_BISHOP:
            return whiteBishopTexture;
        case PieceType::WHITE_ROOK:
            return whiteRookTexture;
        case PieceType::WHITE_KING:
            return whiteKingTexture;
        case PieceType::WHITE_QUEEN:
            return whiteQueenTexture;
        case PieceType::WHITE_PAWN:
            return whitePawnTexture;
        case PieceType::BLACK_KNIGHT:
            return blackKnightTexture;
        case PieceType::BLACK_BISHOP:
            return blackBishopTexture;
        case PieceType::BLACK_ROOK:
            return blackRookTexture;
        case PieceType::BLACK_KING:
            return blackKingTexture;
        case PieceType::BLACK_QUEEN:
            return blackQueenTexture;
        case PieceType::BLACK_PAWN:
            return blackPawnTexture;
        default:
            return Texture2D{}; // Return an empty texture for EMPTY or unknown pieces
    }
}

void ChessAnalysisProgram::renderPieces(){
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            PieceType piece = currentPosition.getPieceAt(row, col);
            Texture2D* textureToDraw = nullptr;

            switch(piece) {
                case PieceType::WHITE_KNIGHT:
                    textureToDraw = &whiteKnightTexture;
                    break;
                case PieceType::WHITE_BISHOP:
                    textureToDraw = &whiteBishopTexture;
                    break;
                case PieceType::WHITE_ROOK:
                    textureToDraw = &whiteRookTexture;
                    break;
                case PieceType::WHITE_KING:
                    textureToDraw = &whiteKingTexture;
                    break;
                case PieceType::WHITE_QUEEN:
                    textureToDraw = &whiteQueenTexture;
                    break;
                case PieceType::WHITE_PAWN:
                    textureToDraw = &whitePawnTexture;
                    break;
                case PieceType::BLACK_KNIGHT:
                    textureToDraw = &blackKnightTexture;
                    break;
                case PieceType::BLACK_BISHOP:
                    textureToDraw = &blackBishopTexture;
                    break;
                case PieceType::BLACK_ROOK:
                    textureToDraw = &blackRookTexture;
                    break;
                case PieceType::BLACK_KING:
                    textureToDraw = &blackKingTexture;
                    break;
                case PieceType::BLACK_QUEEN:
                    textureToDraw = &blackQueenTexture;
                    break;
                case PieceType::BLACK_PAWN:
                    textureToDraw = &blackPawnTexture;
                    break;
            }

            if (textureToDraw != nullptr) {
                Vector2 position = { col * 80.0f, row * 80.0f };
                DrawTextureEx(*textureToDraw, position, 0.0f, 0.65f, WHITE);
            }
        }
    }
}

void ChessAnalysisProgram::UpdateGame(){ // change to updateGame
    // Update game state logic here
}

void ChessAnalysisProgram::RenderGame(){ // change to renderGame
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTextureEx(this->boardTexture, {0, 0}, 0.0f, 0.65f, WHITE);
        renderUI();
        renderBoard();
        renderPieces();
        EndDrawing();
}

void ChessAnalysisProgram::run() {

    // Main program loop
    while (!WindowShouldClose()) { // Detect window close button or ESC key
        UpdateGame();
        RenderGame();
    }

    // De-initialization
}