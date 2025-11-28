# UCI Engine Integration Guide

## Overview

The chess analysis program now includes full UCI (Universal Chess Interface) engine integration, allowing you to analyze positions with a chess engine like Stockfish.

## Features

### 1. Engine Toggle
- **Press X** to enable/disable the engine
- Engine starts **disabled** by default to save resources
- Visual indicator shows engine status (ON/OFF) in the top-right corner

### 2. Real-time Analysis
When enabled, the engine provides:
- **Up to 4 principal variations** (best lines)
- **Evaluation** for each line (in pawns, e.g., +0.43 or Mate in 3)
- **Depth and Selective Depth** information
- **Principal Variation moves** in UCI notation

### 3. Automatic Position Updates
- The engine automatically analyzes the current position
- Updates in real-time as you make moves
- Tracks full move history from the starting position

## Setup Instructions

### 1. Install a UCI Engine

Download Stockfish (recommended):
- Visit: https://stockfishchess.org/download/
- Download the Windows version
- Extract `stockfish.exe` to your project directory

### 2. Configure Engine Path

In `chess_analysis_program.cpp`, line 25:
```cpp
uciEngine = std::make_unique<UCIEngine>("stockfish.exe");
```

Update the path if your engine is in a different location:
```cpp
uciEngine = std::make_unique<UCIEngine>("C:/path/to/stockfish.exe");
```

## Usage

### Basic Workflow

1. **Start the program** - Engine is disabled by default
2. **Make some moves** on the board
3. **Press X** to enable the engine
4. **View analysis** on the right side of the screen

### Understanding the Display

The analysis panel shows up to 4 lines, each containing:

```
Line 1:
Eval: +0.43          <- Position evaluation (positive = White advantage)
Depth: D20/SD28      <- Search depth / Selective depth
PV: e2e4 e7e5 g1f3   <- Best move sequence (UCI notation)
```

#### Evaluation Colors:
- **Green** (+): White is winning
- **Red** (-): Black is winning  
- **Gray** (0.00): Position is equal
- **"Mate in N"**: Forced checkmate in N moves

#### Depth Information:
- **D20**: Main search depth (20 ply)
- **SD28**: Selective depth (deeper in critical lines)
- Higher depth = more accurate analysis

### UCI Move Notation

Moves are shown in UCI format:
- `e2e4` = Pawn from e2 to e4
- `g1f3` = Knight from g1 to f3
- `e7e8q` = Pawn promotion to Queen

## Technical Details

### Architecture

1. **Non-blocking Design**
   - Engine runs in a separate thread
   - Never blocks the UI or game loop
   - Polls for updates each frame

2. **Move History Tracking**
   - Stores all moves from the starting position
   - Sends position as: `position fen <start> moves e2e4 e7e5 ...`
   - Automatically updates engine when moves are made

3. **Multi-PV Analysis**
   - Engine configured for `MultiPV 4`
   - Shows 4 best alternative lines
   - Helps understand multiple good options

### Performance

- **Polling Rate**: Every frame (~120 FPS)
- **Engine Update**: Only when position changes
- **Analysis Mode**: Infinite (continues until stopped)
- **Resource Usage**: Minimal when disabled

## Troubleshooting

### Engine Not Starting

**Symptom**: "Engine: OFF" even after pressing X

**Solutions**:
1. Check engine path is correct
2. Ensure `stockfish.exe` exists
3. Check console for error messages
4. Try absolute path instead of relative

### No Analysis Showing

**Symptom**: "Analyzing..." never changes

**Solutions**:
1. Wait a few seconds (engine needs time)
2. Check if position is legal
3. Verify engine is actually running (check Task Manager)
4. Try making a move to trigger update

### Incorrect Analysis

**Symptom**: Analysis doesn't match position

**Solutions**:
1. Restart the program
2. Check move history is correct
3. Verify starting FEN matches actual position

## Advanced Configuration

### Change Number of Lines

In `uci_engine.cpp`, line 375:
```cpp
sendCommand("setoption name MultiPV value 4");
```

Change `4` to desired number (1-4 recommended)

### Adjust Analysis Depth

The engine uses infinite analysis by default. To limit depth:

In `uci_engine.cpp`, line 238, replace:
```cpp
sendCommand("go infinite");
```

With:
```cpp
sendCommand("go depth 20");  // Limit to depth 20
```

### Change Engine Threads

Add to `initializeEngine()` in `uci_engine.cpp`:
```cpp
sendCommand("setoption name Threads value 4");  // Use 4 CPU threads
```

## Code Structure

### Key Files

- `uci_engine.h/cpp` - UCI protocol implementation
- `chess_analysis_program.h/cpp` - Main program with engine integration
- `chess_game.h/cpp` - Chess logic and move validation

### Key Methods

```cpp
// Enable/disable engine
uciEngine->enable();
uciEngine->disable();

// Update position
uciEngine->setPosition(startingFen, moveHistory);

// Get analysis
EngineAnalysis analysis = uciEngine->pollAnalysis();

// Check status
bool enabled = uciEngine->isEnabled();
```

## Future Enhancements

Possible improvements:
- [ ] Engine strength adjustment
- [ ] Time-limited analysis
- [ ] Save/load analysis
- [ ] Best move highlighting on board
- [ ] Evaluation graph over time
- [ ] Opening book integration
- [ ] Multiple engine support

## Credits

- **UCI Protocol**: Universal Chess Interface standard
- **Stockfish**: Open-source chess engine
- **Raylib**: Graphics library
