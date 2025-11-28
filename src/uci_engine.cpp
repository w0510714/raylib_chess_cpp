#include "uci_engine.h"
#include <iostream>
#include <chrono>
#include <windows.h>
#include <sstream>
#include <regex>
#include <algorithm>

UCIEngine::UCIEngine(const std::string& enginePath)
    : enginePath_(enginePath)
    , state_(EngineState::Disconnected)
    , enabled_(false)
    , processHandle_(nullptr)
    , inputHandle_(nullptr)
    , outputHandle_(nullptr)
{
    // Engine starts disabled - call enable() to start it
}

UCIEngine::~UCIEngine()
{
    if (enabled_) {
        disable();
    }
}


// now for the enable function:
void UCIEngine::enable()
{
    if (enabled_) {
        return; // Already enabled
    }
    
    state_ = EngineState::Connecting;
    
    if (!initializeEngine()) {
        state_ = EngineState::Error;
        return;
    }
    
    // Only set enabled to true AFTER initialization succeeds
    enabled_ = true;
    state_ = EngineState::Ready;
    
    // Start analysis thread
    analysisThread_ = std::make_unique<std::thread>(&UCIEngine::analysisThreadFunction, this);
}

// now for the disable function:
void UCIEngine::disable()
{
    if (!enabled_) {
        return; // Already disabled
    }
    
    enabled_ = false;
    state_ = EngineState::Disconnected;
    
    // Wait for thread to finish
    if (analysisThread_ && analysisThread_->joinable()) {
        analysisThread_->join();
    }
    
    // Close all handles
    if (inputHandle_) {
        CloseHandle(static_cast<HANDLE>(inputHandle_));
    }
    if (outputHandle_) {
        CloseHandle(static_cast<HANDLE>(outputHandle_));
    }
    if (processHandle_) {
        CloseHandle(static_cast<HANDLE>(processHandle_));
    }
    
    // Reset handles
    processHandle_ = nullptr;
    inputHandle_ = nullptr;
    outputHandle_ = nullptr;
    analysisThread_ = nullptr;
}

bool UCIEngine::isEnabled() const
{
    return enabled_;
}

void UCIEngine::setPosition(const std::string& startFen, const std::vector<std::string>& moves)
{
    // Check if engine is enabled
    if (!enabled_) {
        return;
    }
    
    // Validate FEN
    if (!isValidFEN(startFen)) {
        std::cerr << "Invalid starting FEN string: " << startFen << std::endl;
        return;
    }
    
    // Set the requested starting FEN and moves - the analysis thread will detect the change
    {
        std::lock_guard<std::mutex> lock(analysisMutex_);
        requestedStartFen_ = startFen;
        requestedMoves_ = moves;
    }
}

EngineAnalysis UCIEngine::pollAnalysis()
{
    EngineAnalysis result;
    
    // Get current state
    result.state = state_;
    
    // Get latest analysis data
    {
        std::lock_guard<std::mutex> lock(analysisMutex_);
        // Build FEN from start position and moves for display
        result.fen = currentStartFen_; // Could build actual current FEN if needed
        result.rawInfo = currentAnalysis_.rawInfo;
        result.hasResult = currentAnalysis_.hasResult;
        result.lines = currentAnalysis_.lines;
    }
    
    return result;
}

void UCIEngine::analysisThreadFunction()
{
    while (true) {
        // Exit if disabled
        if (!enabled_) {
            break;
        }
        
        EngineState currentState = state_;
        
        // Exit if disconnected or error
        if (currentState == EngineState::Disconnected || currentState == EngineState::Error) {
            break;
        }
        
        // Check for and handle any position change requests
        handlePositionTransition();
        
        // Read engine output while analyzing
        if (currentState == EngineState::Analyzing) {
            // Only attempt to read if data is available to avoid blocking
            if (hasDataAvailable()) {
                readEngineOutput();
            }
        }
        
        // Sleep based on current state (stale state is acceptable, will refresh next iteration)
        std::this_thread::sleep_for(currentState == EngineState::Analyzing ? 
            std::chrono::milliseconds(10) : 
            std::chrono::milliseconds(100));
    }
}

void UCIEngine::handlePositionTransition()
{
    // Check if there's a position change to handle
    std::string requestedStartFen;
    std::vector<std::string> requestedMoves;
    std::string currentStartFen;
    std::vector<std::string> currentMoves;
    {
        std::lock_guard<std::mutex> lock(analysisMutex_);
        requestedStartFen = requestedStartFen_;
        requestedMoves = requestedMoves_;
        currentStartFen = currentStartFen_;
        currentMoves = currentMoves_;
    }
    
    // If no change requested or already analyzing this position, nothing to do
    if (requestedStartFen.empty() || 
        (requestedStartFen == currentStartFen && requestedMoves == currentMoves)) {
        return;
    }
    
    // Need to transition to new position - stop current analysis if running
    if (state_ == EngineState::Analyzing) {
        stopCurrentAnalysis();
    }
    
    // Update current position and clear results
    {
        std::lock_guard<std::mutex> lock(analysisMutex_);
        currentStartFen_ = requestedStartFen;
        currentMoves_ = requestedMoves;
        currentAnalysis_ = EngineAnalysis();
    }
    // Start analysis on new position
    startAnalysisForPosition(requestedStartFen, requestedMoves);
}

void UCIEngine::stopCurrentAnalysis()
{
    state_ = EngineState::Stopping;
    sendCommand("stop");
    
    // Wait for stop to complete by reading output until we see "bestmove"
    std::string line;
    do {
        line = readResponseLine();
    } while (!line.empty() && line.find("bestmove") == std::string::npos);
}

void UCIEngine::startAnalysisForPosition(const std::string& startFen, const std::vector<std::string>& moves)
{
    const std::string standardStartpos = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    
    // Send ucinewgame if this is the start of a new game (no moves and at standard starting position)
    if (moves.empty() && startFen == standardStartpos) {
        sendCommand("ucinewgame");
    }
    
    // Build position command: "position startpos moves e2e4 e7e5 ..."
    // or: "position fen <fen> moves e2e4 e7e5 ..."
    std::string positionCommand;
    if (startFen == standardStartpos) {
        positionCommand = "position startpos";
    } else {
        positionCommand = "position fen " + startFen;
    }
    
    // Append moves if any
    if (!moves.empty()) {
        positionCommand += " moves";
        for (const auto& move : moves) {
            positionCommand += " " + move;
        }
    }
    
    sendCommand(positionCommand);
    sendCommand("go infinite");
    state_ = EngineState::Analyzing;
}

void UCIEngine::readEngineOutput()
{
    // Peek first to avoid blocking if no data available
    if (!hasDataAvailable()) {
        return;
    }
    
    // Read one line and process it
    std::string output = readResponseLine();
    if (!output.empty()) {
        parseEngineOutput(output);
    }
}

bool UCIEngine::sendCommand(const std::string& command)
{
    HANDLE handle = static_cast<HANDLE>(inputHandle_);
    if (!handle) return false;
    
    DWORD bytesWritten;
    std::string fullCommand = command + "\n";
    
    if (WriteFile(handle, fullCommand.c_str(), fullCommand.length(), &bytesWritten, nullptr)) {
        std::cout << "Sent: " << command << std::endl;
        return true;
    }
    return false;
}

bool UCIEngine::hasDataAvailable() const
{
    HANDLE handle = static_cast<HANDLE>(outputHandle_);
    if (!handle) return false;
    
    DWORD bytesAvailable = 0;
    // PeekNamedPipe checks if data is available without reading it
    // Returns true if there's data available, false otherwise
    if (PeekNamedPipe(handle, nullptr, 0, nullptr, &bytesAvailable, nullptr)) {
        return bytesAvailable > 0;
    }
    return false;
}

std::string UCIEngine::readResponseLine()
{
    std::string response;
    
    HANDLE handle = static_cast<HANDLE>(outputHandle_);
    if (!handle) return "";
    
    while (true) {
        char ch;
        DWORD bytesRead;
        
        if (ReadFile(handle, &ch, 1, &bytesRead, nullptr) && bytesRead == 1) {
            response += ch;
            
            if (ch == '\n') {
                break;
            }
        } else {
            break;
        }
    }
    
    return response;
}

bool UCIEngine::initializeEngine()
{
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {};
    
    HANDLE hInputRead, hInputWrite, hOutputRead, hOutputWrite;
    SECURITY_ATTRIBUTES sa = { sizeof(sa), nullptr, TRUE };
    
    if (!CreatePipe(&hInputRead, &hInputWrite, &sa, 0) ||
        !CreatePipe(&hOutputRead, &hOutputWrite, &sa, 0)) {
        return false;
    }
    
    si.hStdInput = hInputRead;
    si.hStdOutput = hOutputWrite;
    si.hStdError = hOutputWrite;
    si.dwFlags = STARTF_USESTDHANDLES;
    
    std::wstring widePath(enginePath_.begin(), enginePath_.end());
    if (!CreateProcessW(nullptr, const_cast<wchar_t*>(widePath.c_str()),
                       nullptr, nullptr, TRUE, 0, nullptr, nullptr,
                       &si, &pi)) {
        CloseHandle(hInputRead);
        CloseHandle(hInputWrite);
        CloseHandle(hOutputRead);
        CloseHandle(hOutputWrite);
        return false;
    }
    
    processHandle_ = pi.hProcess;
    inputHandle_ = hInputWrite;
    outputHandle_ = hOutputRead;
    
    CloseHandle(hInputRead);
    CloseHandle(hOutputWrite);
    CloseHandle(pi.hThread);
    
    // Send UCI command
    if (!sendCommand("uci")) {
        return false;
    }
    
    // Wait for uciok
    while (true) {
        std::string line = readResponseLine();
        if (!line.empty() && line.find("uciok") != std::string::npos) {
            break;
        }
    }
    
    // Send isready
    if (!sendCommand("isready")) {
        return false;
    }
    
    // Wait for readyok
    while (true) {
        std::string line = readResponseLine();
        if (!line.empty() && line.find("readyok") != std::string::npos) {
            break;
        }
    }
    
    // Set MultiPV to 4 for multiple principal variations
    if (!sendCommand("setoption name MultiPV value 4")) {
        return false;
    }
    
    return true;
}


EngineAnalysis::AnalysisLine UCIEngine::parseAnalysisLine(const std::string& line) const
{
    EngineAnalysis::AnalysisLine analysisLine;
    std::smatch match;
    // multipv (default 1)
    if (std::regex_search(line, match, std::regex(R"(multipv\s+(\d+))"))) {
        analysisLine.multipv = std::stoi(match[1]);
    } else {
        analysisLine.multipv = 1;
    }
    analysisLine.text = line;
    return analysisLine;
}

void UCIEngine::parseEngineOutput(const std::string& output)
{
    // Process single line directly (readResponseLine always returns one line)
    std::string line = trim(output);
    
    // Early return if empty
    if (line.empty()) {
        return;
    }
    
    // Ignore lines containing "currmove"
    if ( line.find("currmove") != std::string::npos) {
        return;
    }
    
    // Check if it's an info line first (no lock needed for this check)
    bool isInfoLine = (line.rfind("info", 0) == 0);
    
    if (!isInfoLine) {
        // Not an info line - just update rawInfo
        std::lock_guard<std::mutex> lock(analysisMutex_);
        currentAnalysis_.rawInfo = line;
        return;
    }
    
    // This is an info line - do parsing work outside the lock
    EngineAnalysis::AnalysisLine analysisLine = parseAnalysisLine(line);
    
    // Update shared state (critical section)
    std::lock_guard<std::mutex> lock(analysisMutex_);
    currentAnalysis_.rawInfo = line;
    currentAnalysis_.hasResult = true;
    
    // If we see multipv 1, clear all existing lines (new batch/depth)
    if (analysisLine.multipv == 1) {
        currentAnalysis_.lines.clear();
    }
    
    // Merge into currentAnalysis_.lines by multipv (up to 4)
    bool found = false;
    for (auto& existing : currentAnalysis_.lines) {
        if (existing.multipv == analysisLine.multipv) {
            existing = analysisLine;
            found = true;
            break;
        }
    }
    if (!found && currentAnalysis_.lines.size() < 4) {
        currentAnalysis_.lines.push_back(analysisLine);
    }
}

std::string UCIEngine::trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

bool UCIEngine::isValidFEN(const std::string& fen)
{
    if (fen.empty()) return false;
    
    std::istringstream stream(fen);
    std::string part;
    int partCount = 0;
    
    while (std::getline(stream, part, ' ')) {
        partCount++;
    }
    
    return partCount == 6;
}


