# A Simple Chess Game
#
# The part 3 of a 3 part assignment
#
# Run w/ Raylib (I use F5)
#
# Extra Assets in assets and images
#

## UCI Engine Integration

The program now includes full UCI chess engine support for position analysis!

### Quick Start
1. Download Stockfish from https://stockfishchess.org/download/
2. Place `stockfish.exe` in the project directory
3. Run the program and press **X** to enable the engine
4. View real-time analysis on the right side of the screen

### Features
- **Press X** to toggle engine on/off
- **Up to 4 best lines** displayed with evaluation
- **Depth and selective depth** information
- **Principal variation** moves shown in UCI notation
- **Automatic position updates** as you make moves

See `UCI_ENGINE_GUIDE.md` for detailed documentation.

## FEN Position Loading

On startup, the program will attempt to load a chess position from `src/initial_position.fen`.

- If the file exists and contains valid FEN notation, that position will be loaded
- If the file is missing or contains invalid FEN, the standard starting position will be used
- The program will print status messages indicating which option was used

### FEN Format

The FEN file should contain a single line with standard FEN notation:
```
[piece placement] [active color] [castling rights] [en passant] [halfmove clock] [fullmove number]
```

Example (current position in file):
```
7k/6p1/6Pp/7P/p7/5B2/PP4K1/8 w - - 0 1
```

This represents an endgame position with White to move.

### Standard Starting Position FEN
```
rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1
```

#
# More fixes I could do:
#   Add more helped functions
#   Reduce copied code
#   Make cleaner & better formatting
#   More comments for clarity
# 
# Things I wanted to add but didn't get around to:
#   Visually saying what game state it is (checkmate)
#   Highlighted (orange/red) for check and pinned pieces
#   Changing the chess pieces and board
#   Threefold repetition (was removed from requirements)
#
# I just wanted the neccesities done first