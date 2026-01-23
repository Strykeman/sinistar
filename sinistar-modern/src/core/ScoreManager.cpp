/**
 * ScoreManager.cpp
 *
 * Implementation of scoring system
 */

#include "ScoreManager.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <cmath>

ScoreManager::ScoreManager()
    : currentScore_(0)
    , currentLevel_(1)
    , killCount_(0)
    , shotsFired_(0)
    , shotsHit_(0)
    , scoreForNextLevel_(10000)
    , baseScorePerLevel_(10000)
    , levelTime_(0.0f)
    , maxLevelTime_(120.0f)  // 2 minutes per level
    , highScoreFile_("highscores.dat")
{
    initializeDefaultHighScores();
    loadHighScores();
}

ScoreManager::~ScoreManager() {
    saveHighScores();
}

void ScoreManager::reset() {
    currentScore_ = 0;
    currentLevel_ = 1;
    killCount_ = 0;
    shotsFired_ = 0;
    shotsHit_ = 0;
    levelTime_ = 0.0f;
    scoreForNextLevel_ = calculateNextLevelScore();

    std::cout << "Score manager reset" << std::endl;
}

void ScoreManager::addScore(int points) {
    currentScore_ += points;

    // Check for level advancement
    if (shouldAdvanceLevel()) {
        advanceLevel();
    }
}

void ScoreManager::update(float deltaTime) {
    levelTime_ += deltaTime;

    // Optional: Auto-advance level after max time
    // Disabled for now - only advance by score
    /*
    if (levelTime_ >= maxLevelTime_) {
        advanceLevel();
    }
    */
}

bool ScoreManager::shouldAdvanceLevel() const {
    return currentScore_ >= scoreForNextLevel_;
}

void ScoreManager::advanceLevel() {
    currentLevel_++;
    levelTime_ = 0.0f;
    scoreForNextLevel_ = calculateNextLevelScore();

    std::cout << "Advanced to level " << currentLevel_ << std::endl;
    std::cout << "Score needed for next level: " << scoreForNextLevel_ << std::endl;
}

float ScoreManager::getAccuracy() const {
    if (shotsFired_ == 0) return 0.0f;
    return static_cast<float>(shotsHit_) / static_cast<float>(shotsFired_);
}

bool ScoreManager::isHighScore(int score) const {
    if (highScores_.size() < static_cast<size_t>(MAX_HIGH_SCORES)) {
        return true;  // List not full yet
    }

    // Check if score beats lowest high score
    return score > highScores_.back().score;
}

void ScoreManager::addHighScore(const std::string& name, int score, int level) {
    HighScoreEntry entry(name, score, level);
    highScores_.push_back(entry);

    // Sort by score (descending)
    std::sort(highScores_.begin(), highScores_.end(),
        [](const HighScoreEntry& a, const HighScoreEntry& b) {
            return a.score > b.score;
        });

    // Keep only top scores
    if (highScores_.size() > static_cast<size_t>(MAX_HIGH_SCORES)) {
        highScores_.resize(MAX_HIGH_SCORES);
    }

    // Save to file
    saveHighScores();
}

bool ScoreManager::loadHighScores() {
    std::ifstream file(highScoreFile_, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "No high score file found, using defaults" << std::endl;
        return false;
    }

    highScores_.clear();

    int count = 0;
    file.read(reinterpret_cast<char*>(&count), sizeof(int));

    for (int i = 0; i < count && i < MAX_HIGH_SCORES; i++) {
        HighScoreEntry entry;

        // Read name (fixed 4 chars)
        char nameBuffer[4];
        file.read(nameBuffer, 4);
        entry.name = std::string(nameBuffer, 3);  // 3 chars + null terminator

        file.read(reinterpret_cast<char*>(&entry.score), sizeof(int));
        file.read(reinterpret_cast<char*>(&entry.level), sizeof(int));

        highScores_.push_back(entry);
    }

    file.close();

    std::cout << "Loaded " << highScores_.size() << " high scores" << std::endl;
    return true;
}

bool ScoreManager::saveHighScores() {
    std::ofstream file(highScoreFile_, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to save high scores" << std::endl;
        return false;
    }

    int count = static_cast<int>(highScores_.size());
    file.write(reinterpret_cast<const char*>(&count), sizeof(int));

    for (const auto& entry : highScores_) {
        // Write name (fixed 4 chars: 3 letters + null)
        char nameBuffer[4] = {0};
        for (size_t i = 0; i < 3 && i < entry.name.length(); i++) {
            nameBuffer[i] = entry.name[i];
        }
        file.write(nameBuffer, 4);

        file.write(reinterpret_cast<const char*>(&entry.score), sizeof(int));
        file.write(reinterpret_cast<const char*>(&entry.level), sizeof(int));
    }

    file.close();
    return true;
}

float ScoreManager::getDifficultyMultiplier() const {
    // Increase difficulty by 10% per level, capped at 3x
    float multiplier = 1.0f + (currentLevel_ - 1) * 0.1f;
    return std::min(multiplier, 3.0f);
}

float ScoreManager::getEnemySpawnRate() const {
    // Spawn enemies faster as levels increase
    // Base: 2.0 seconds, min: 0.5 seconds
    float baseRate = 2.0f;
    float reduction = (currentLevel_ - 1) * 0.1f;
    return std::max(baseRate - reduction, 0.5f);
}

int ScoreManager::getMaxEnemies() const {
    // More enemies per level
    // Base: 10, increase by 2 per level, cap at 30
    int maxEnemies = 10 + (currentLevel_ - 1) * 2;
    return std::min(maxEnemies, 30);
}

int ScoreManager::calculateNextLevelScore() const {
    // Each level requires more score
    // Level 1: 10000, Level 2: 25000, Level 3: 45000, etc.
    return baseScorePerLevel_ * currentLevel_ + (currentLevel_ - 1) * 5000;
}

void ScoreManager::initializeDefaultHighScores() {
    highScores_.clear();

    // Default high scores (arcade style)
    highScores_.push_back(HighScoreEntry("SIN", 100000, 10));
    highScores_.push_back(HighScoreEntry("WAR", 75000, 8));
    highScores_.push_back(HighScoreEntry("CRY", 50000, 6));
    highScores_.push_back(HighScoreEntry("BOB", 30000, 5));
    highScores_.push_back(HighScoreEntry("TED", 20000, 4));
    highScores_.push_back(HighScoreEntry("JOE", 15000, 3));
    highScores_.push_back(HighScoreEntry("SAM", 10000, 2));
    highScores_.push_back(HighScoreEntry("ACE", 7500, 2));
    highScores_.push_back(HighScoreEntry("MAX", 5000, 1));
    highScores_.push_back(HighScoreEntry("ZAK", 2500, 1));
}
