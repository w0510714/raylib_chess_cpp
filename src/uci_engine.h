#pragma once

#include <string>
#include <memory>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <utility>

/**
 * Simple non-blocking UCI engine wrapper
 * 
 * Key features:
 * - Completely non-blocking API for polling
 * - Internal state machine handles stopping/resetting analysis
 * - Clean separation: you poll for updates, we manage the engine
 */

enum class EngineState {
    Disconnected,   // Not connected to engine
    Connecting,     // Establishing connection to engine
    Ready,          // Connected and ready for commands (not analyzing)
    Stopping,       // Stopping current analysis (transition state)
    Analyzing,      // Currently analyzing a position
    Error           // Error occurred
};

struct EngineAnalysis {
    EngineState state;              // Current engine state
    
    // Validity flag - indicates if analysis data is valid/available
    bool hasResult = false;
    
    // Analysis data - only valid when hasResult is true
    std::string fen;                // Position relevant to the analysis
    std::string rawInfo;            // data for printing to the screen
    
    // MultiPV analysis lines (up to 4) - simplified string-based representation
    struct AnalysisLine {
        int multipv = 1;            // Principal variation number (1-4)
        std::string text;           // Raw info line (trimmed) for this PV
    };
    
    std::vector<AnalysisLine> lines; // Up to 4 analysis lines
};

class UCIEngine {
public:
    UCIEngine(const std::string& enginePath);
    ~UCIEngine();
    
    // Non-copyable
    UCIEngine(const UCIEngine&) = delete;
    UCIEngine& operator=(const UCIEngine&) = delete;
    
    /**
     * Set the position to analyze
     * This is non-blocking and returns immediately.
     * Internal state machine will handle stopping current analysis if needed.
     * Call pollAnalysis() to get updates.
     * 
     * @param startFen The starting position FEN (where the game aka the move sequence began)
     * @param moves The list of moves from startFen to the current position
     */
    void setPosition(const std::string& startFen, const std::vector<std::string>& moves);

    
    /**
     * Poll for current analysis state
     * This updates internal state and returns latest analysis.
     * Should be called regularly (e.g., every frame in raylib).
     * Returns immediately, never blocks.
     * 
     * Gracefully handles situations where engine has no result yet:
     * - The hasResult field indicates if analysis data is valid
     * - When hasResult==false, do not access other analysis data fields
     * - Never throws exceptions
     * - State field indicates what engine is currently doing
     */
    EngineAnalysis pollAnalysis();
    
    /**
     * Enable the engine
     * Connects to and initializes the engine if not already connected.
     */
    void enable();
    
    /**
     * Disable the engine
     * Stops all analysis and disconnects from the engine.
     */
    void disable();
    
    /**
     * Check if the engine is currently enabled
     */
    bool isEnabled() const;
    
private:
    // Engine process management
    std::string enginePath_;
    std::atomic<EngineState> state_;
    std::atomic<bool> enabled_;
    
    // Process communication
    std::unique_ptr<std::thread> analysisThread_;
    std::mutex analysisMutex_;
    EngineAnalysis currentAnalysis_;
    
    // Position tracking - protected by analysisMutex_
    std::string requestedStartFen_;      // Starting FEN requested by setPosition()
    std::vector<std::string> requestedMoves_; // Moves requested by setPosition()
    std::string currentStartFen_;        // Starting FEN currently being analyzed
    std::vector<std::string> currentMoves_;  // Moves currently being analyzed
    
    // Engine process handles (Windows-specific)
    void* processHandle_;
    void* inputHandle_;
    void* outputHandle_;
    
    // Engine communication helpers
    bool sendCommand(const std::string& command);
    std::string readResponseLine();
    bool hasDataAvailable() const;
    bool initializeEngine();
    
    // Analysis thread function
    void analysisThreadFunction();
    
    // Analysis thread helpers
    void handlePositionTransition();
    void startAnalysisForPosition(const std::string& startFen, const std::vector<std::string>& moves);
    void stopCurrentAnalysis();
    void readEngineOutput();
    
    // Parsing helpers
    void parseEngineOutput(const std::string& output);
    std::pair<bool, int> isInfoLineWithMultipv(const std::string& line) const;
    EngineAnalysis::AnalysisLine parseAnalysisLine(const std::string& line) const;
    std::string trim(const std::string& str);
    bool isValidFEN(const std::string& fen);
};

