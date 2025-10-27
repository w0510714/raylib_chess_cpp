#pragma once

#include <raylib.h>
#include "chess_game.h"

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
    // Loads textures for board and pieces
    void RenderGame();
    void UpdateGame();
    void renderUI();
    void renderBoard();
    void renderPieces();
    void getTextureForPiece();
    void loadAllTextures();
    void unloadAllTextures();
    Texture2D getTextureForPiece(PieceType Place);
    
    
    
};
