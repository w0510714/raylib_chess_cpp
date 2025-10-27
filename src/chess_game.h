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
    ChessGame();
    ~ChessGame() = default;

    PieceType getPieceAt(int row, int col) const;
    bool makeMove(int startRow, int startCol, int endRow, int endCol);
    GameStatus getGameStatus() const { return status; }
    bool isWhiteTurn() const { return whiteTurn; }

private:
    PieceType board[8][8];
    bool whiteTurn;
    GameStatus status;

    void initializeBoard();
};