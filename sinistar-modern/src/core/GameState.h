/**
 * GameState.h
 *
 * Game state manager for handling different game modes
 * (menu, playing, paused, game over)
 */

#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <SDL2/SDL.h>
#include <functional>
#include <string>

/**
 * Game state enumeration
 */
enum class GameState {
    MENU,       // Main menu
    PLAYING,    // Active gameplay
    PAUSED,     // Game paused
    GAME_OVER,  // Game over screen
    QUIT        // Quit application
};

/**
 * Menu item for menu screens
 */
struct MenuItem {
    std::string text;
    std::function<void()> action;
    bool enabled;

    MenuItem(const std::string& t, std::function<void()> a, bool e = true)
        : text(t), action(a), enabled(e) {}
};

/**
 * GameStateManager - manages game state transitions and UI
 */
class GameStateManager {
public:
    GameStateManager();
    ~GameStateManager();

    /**
     * Update state logic
     */
    void update(float deltaTime);

    /**
     * Render state UI
     */
    void render(SDL_Renderer* renderer, int screenWidth, int screenHeight);

    /**
     * Handle input for current state
     */
    void handleInput();

    /**
     * Get current state
     */
    GameState getState() const { return currentState_; }

    /**
     * Set state
     */
    void setState(GameState state);

    /**
     * Transition to next state
     */
    void transitionTo(GameState newState);

    /**
     * Set game over reason
     */
    void setGameOverReason(const std::string& reason) { gameOverReason_ = reason; }

    /**
     * Set final score
     */
    void setFinalScore(int score) { finalScore_ = score; }

    /**
     * Get final score
     */
    int getFinalScore() const { return finalScore_; }

    /**
     * Check if game is active (playing)
     */
    bool isPlaying() const { return currentState_ == GameState::PLAYING; }

    /**
     * Check if game is paused
     */
    bool isPaused() const { return currentState_ == GameState::PAUSED; }

    /**
     * Set callbacks for state transitions
     */
    void setStartGameCallback(std::function<void()> callback) { startGameCallback_ = callback; }
    void setResumeGameCallback(std::function<void()> callback) { resumeGameCallback_ = callback; }
    void setQuitCallback(std::function<void()> callback) { quitCallback_ = callback; }

private:
    /**
     * Initialize menu items
     */
    void initializeMenus();

    /**
     * Render main menu
     */
    void renderMainMenu(SDL_Renderer* renderer, int screenWidth, int screenHeight);

    /**
     * Render pause menu
     */
    void renderPauseMenu(SDL_Renderer* renderer, int screenWidth, int screenHeight);

    /**
     * Render game over screen
     */
    void renderGameOver(SDL_Renderer* renderer, int screenWidth, int screenHeight);

    /**
     * Handle menu input
     */
    void handleMenuInput();

    /**
     * Execute selected menu item
     */
    void executeMenuItem();

    /**
     * Move menu selection
     */
    void moveSelection(int direction);

    // Current state
    GameState currentState_;
    GameState previousState_;

    // Menu state
    std::vector<MenuItem> mainMenuItems_;
    std::vector<MenuItem> pauseMenuItems_;
    std::vector<MenuItem> gameOverMenuItems_;
    int currentSelection_;

    // Game over state
    std::string gameOverReason_;
    int finalScore_;

    // Animation
    float menuBlinkTimer_;
    bool menuBlinkState_;

    // Callbacks
    std::function<void()> startGameCallback_;
    std::function<void()> resumeGameCallback_;
    std::function<void()> quitCallback_;
};

#endif // GAMESTATE_H
