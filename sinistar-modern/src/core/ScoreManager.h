/**
 * ScoreManager.h
 *
 * Scoring system with high score tracking and level progression
 */

#ifndef SCOREMANAGER_H
#define SCOREMANAGER_H

#include <string>
#include <vector>

/**
 * High score entry
 */
struct HighScoreEntry {
    std::string name;
    int score;
    int level;

    HighScoreEntry(const std::string& n = "AAA", int s = 0, int l = 1)
        : name(n), score(s), level(l) {}
};

/**
 * ScoreManager - manages scoring, levels, and high scores
 */
class ScoreManager {
public:
    ScoreManager();
    ~ScoreManager();

    /**
     * Reset for new game
     */
    void reset();

    /**
     * Add points to score
     */
    void addScore(int points);

    /**
     * Get current score
     */
    int getScore() const { return currentScore_; }

    /**
     * Get current level
     */
    int getLevel() const { return currentLevel_; }

    /**
     * Update level progression
     */
    void update(float deltaTime);

    /**
     * Check if should advance level
     */
    bool shouldAdvanceLevel() const;

    /**
     * Advance to next level
     */
    void advanceLevel();

    /**
     * Get enemy kill count
     */
    int getKillCount() const { return killCount_; }

    /**
     * Increment kill count
     */
    void addKill() { killCount_++; }

    /**
     * Get accuracy (shots hit / shots fired)
     */
    float getAccuracy() const;

    /**
     * Record shot fired
     */
    void recordShotFired() { shotsFired_++; }

    /**
     * Record shot hit
     */
    void recordShotHit() { shotsHit_++; }

    /**
     * Get high scores
     */
    const std::vector<HighScoreEntry>& getHighScores() const { return highScores_; }

    /**
     * Check if score qualifies for high score list
     */
    bool isHighScore(int score) const;

    /**
     * Add high score entry
     */
    void addHighScore(const std::string& name, int score, int level);

    /**
     * Load high scores from file
     */
    bool loadHighScores();

    /**
     * Save high scores to file
     */
    bool saveHighScores();

    /**
     * Get difficulty multiplier for current level
     */
    float getDifficultyMultiplier() const;

    /**
     * Get enemy spawn rate for current level
     */
    float getEnemySpawnRate() const;

    /**
     * Get max enemies for current level
     */
    int getMaxEnemies() const;

private:
    // Current game stats
    int currentScore_;
    int currentLevel_;
    int killCount_;
    int shotsFired_;
    int shotsHit_;

    // Level progression
    int scoreForNextLevel_;
    int baseScorePerLevel_;
    float levelTime_;
    float maxLevelTime_;

    // High scores
    std::vector<HighScoreEntry> highScores_;
    const int MAX_HIGH_SCORES = 10;
    std::string highScoreFile_;

    /**
     * Calculate score needed for next level
     */
    int calculateNextLevelScore() const;

    /**
     * Initialize default high scores
     */
    void initializeDefaultHighScores();
};

#endif // SCOREMANAGER_H
