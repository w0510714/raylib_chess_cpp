#pragma

// enum for different chess pieces
enum class PieceType {
    EMPTY,
    WHITE_PAWN,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_ROOK,
    WHITE_QUEEN,
    WHITE_KING,
    BLACK_PAWN,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_ROOK,
    BLACK_QUEEN,
    BLACK_KING
};

enum GameStatus {
    ONGOING,
    CHECK,
    CHECKMATE,
    STALEMATE,
    DRAW
};

class ChessGame {
    public:
        ChessGame(); // starts a new game
        ~ChessGame();

        // Returns the piece at the given position
        char getPieceAt(int row, int col) const;

        // Attempts to make a move; returns true if successful
        bool makeMove(int startRow, int startCol, int endRow, int endCol);

        // Returns the current status of the game
        GameStatus getGameStatus() const;

        // Returns true if it's white's turn, false otherwise
        bool isWhiteTurn() const;

    private:
        // 8x8 board representation
        PieceType board[8][8];
        bool whiteTurn;
        GameStatus status;


        void initializeBoard();
};