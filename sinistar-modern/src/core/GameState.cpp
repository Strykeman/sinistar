/**
 * GameState.cpp
 *
 * Implementation of game state manager
 */

#include "GameState.h"
#include "../input/InputSystem.h"
#include "../graphics/TextRenderer.h"
#include "../graphics/Color.h"
#include <iostream>

GameStateManager::GameStateManager()
    : currentState_(GameState::MENU)
    , previousState_(GameState::MENU)
    , currentSelection_(0)
    , gameOverReason_("GAME OVER")
    , finalScore_(0)
    , menuBlinkTimer_(0.0f)
    , menuBlinkState_(true)
{
    initializeMenus();
}

GameStateManager::~GameStateManager() {
}

void GameStateManager::update(float deltaTime) {
    // Update menu blink animation
    menuBlinkTimer_ += deltaTime;
    if (menuBlinkTimer_ >= 0.5f) {
        menuBlinkTimer_ = 0.0f;
        menuBlinkState_ = !menuBlinkState_;
    }
}

void GameStateManager::render(SDL_Renderer* renderer, int screenWidth, int screenHeight) {
    switch (currentState_) {
        case GameState::MENU:
            renderMainMenu(renderer, screenWidth, screenHeight);
            break;

        case GameState::PAUSED:
            renderPauseMenu(renderer, screenWidth, screenHeight);
            break;

        case GameState::GAME_OVER:
            renderGameOver(renderer, screenWidth, screenHeight);
            break;

        case GameState::PLAYING:
            // No overlay for playing state
            break;

        case GameState::QUIT:
            break;
    }
}

void GameStateManager::handleInput() {
    InputSystem& input = InputSystem::getInstance();

    switch (currentState_) {
        case GameState::MENU:
        case GameState::PAUSED:
        case GameState::GAME_OVER:
            handleMenuInput();
            break;

        case GameState::PLAYING:
            // Check for pause
            if (input.isButtonPressed(InputButton::PAUSE)) {
                transitionTo(GameState::PAUSED);
            }
            break;

        case GameState::QUIT:
            break;
    }
}

void GameStateManager::setState(GameState state) {
    previousState_ = currentState_;
    currentState_ = state;
    currentSelection_ = 0;

    std::cout << "Game state changed to: ";
    switch (state) {
        case GameState::MENU: std::cout << "MENU"; break;
        case GameState::PLAYING: std::cout << "PLAYING"; break;
        case GameState::PAUSED: std::cout << "PAUSED"; break;
        case GameState::GAME_OVER: std::cout << "GAME_OVER"; break;
        case GameState::QUIT: std::cout << "QUIT"; break;
    }
    std::cout << std::endl;
}

void GameStateManager::transitionTo(GameState newState) {
    setState(newState);
}

void GameStateManager::initializeMenus() {
    // Main menu
    mainMenuItems_.clear();
    mainMenuItems_.push_back(MenuItem("START GAME", [this]() {
        if (startGameCallback_) startGameCallback_();
        transitionTo(GameState::PLAYING);
    }));
    mainMenuItems_.push_back(MenuItem("QUIT", [this]() {
        if (quitCallback_) quitCallback_();
        transitionTo(GameState::QUIT);
    }));

    // Pause menu
    pauseMenuItems_.clear();
    pauseMenuItems_.push_back(MenuItem("RESUME", [this]() {
        if (resumeGameCallback_) resumeGameCallback_();
        transitionTo(GameState::PLAYING);
    }));
    pauseMenuItems_.push_back(MenuItem("QUIT TO MENU", [this]() {
        transitionTo(GameState::MENU);
    }));

    // Game over menu
    gameOverMenuItems_.clear();
    gameOverMenuItems_.push_back(MenuItem("PLAY AGAIN", [this]() {
        if (startGameCallback_) startGameCallback_();
        transitionTo(GameState::PLAYING);
    }));
    gameOverMenuItems_.push_back(MenuItem("MAIN MENU", [this]() {
        transitionTo(GameState::MENU);
    }));
    gameOverMenuItems_.push_back(MenuItem("QUIT", [this]() {
        if (quitCallback_) quitCallback_();
        transitionTo(GameState::QUIT);
    }));
}

void GameStateManager::renderMainMenu(SDL_Renderer* renderer, int screenWidth, int screenHeight) {
    // Semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200);
    SDL_Rect overlay = {0, 0, screenWidth, screenHeight};
    SDL_RenderFillRect(renderer, &overlay);

    // Title
    const char* title = "SINISTAR";
    int titleY = screenHeight / 4;

    // Draw title shadow
    SDL_SetRenderDrawColor(renderer, 50, 0, 0, 255);
    for (int x = 0; x < static_cast<int>(strlen(title)); x++) {
        int charX = screenWidth / 2 - (strlen(title) * 6) / 2 + x * 6 + 2;
        // Shadow offset
        SDL_Rect charRect = {charX, titleY + 2, 6, 8};
        SDL_RenderFillRect(renderer, &charRect);
    }

    // Draw title (would use TextRenderer in actual implementation)
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int x = 0; x < static_cast<int>(strlen(title)); x++) {
        int charX = screenWidth / 2 - (strlen(title) * 6) / 2 + x * 6;
        SDL_Rect charRect = {charX, titleY, 6, 8};
        SDL_RenderFillRect(renderer, &charRect);
    }

    // Menu items
    int menuY = screenHeight / 2;
    for (size_t i = 0; i < mainMenuItems_.size(); i++) {
        int itemY = menuY + static_cast<int>(i) * 20;

        // Selection indicator
        if (static_cast<int>(i) == currentSelection_ && menuBlinkState_) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_Rect indicator = {screenWidth / 2 - 60, itemY, 4, 8};
            SDL_RenderFillRect(renderer, &indicator);
        }

        // Menu item text (simplified - would use TextRenderer)
        Color itemColor = (static_cast<int>(i) == currentSelection_) ? Color::YELLOW : Color::WHITE;
        SDL_SetRenderDrawColor(renderer, itemColor.r, itemColor.g, itemColor.b, itemColor.a);

        int textWidth = static_cast<int>(mainMenuItems_[i].text.length() * 4);
        int textX = screenWidth / 2 - textWidth / 2;
        SDL_Rect textRect = {textX, itemY, textWidth, 6};
        SDL_RenderFillRect(renderer, &textRect);
    }

    // Controls hint
    const char* hint = "UP/DOWN: SELECT  SPACE/ENTER: CONFIRM";
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255);
    int hintWidth = static_cast<int>(strlen(hint) * 3);
    int hintX = screenWidth / 2 - hintWidth / 2;
    SDL_Rect hintRect = {hintX, screenHeight - 30, hintWidth, 4};
    SDL_RenderFillRect(renderer, &hintRect);
}

void GameStateManager::renderPauseMenu(SDL_Renderer* renderer, int screenWidth, int screenHeight) {
    // Semi-transparent overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
    SDL_Rect overlay = {0, 0, screenWidth, screenHeight};
    SDL_RenderFillRect(renderer, &overlay);

    // "PAUSED" title
    const char* title = "PAUSED";
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    int titleWidth = static_cast<int>(strlen(title) * 6);
    int titleX = screenWidth / 2 - titleWidth / 2;
    SDL_Rect titleRect = {titleX, screenHeight / 3, titleWidth, 8};
    SDL_RenderFillRect(renderer, &titleRect);

    // Menu items
    int menuY = screenHeight / 2;
    for (size_t i = 0; i < pauseMenuItems_.size(); i++) {
        int itemY = menuY + static_cast<int>(i) * 20;

        if (static_cast<int>(i) == currentSelection_ && menuBlinkState_) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_Rect indicator = {screenWidth / 2 - 60, itemY, 4, 8};
            SDL_RenderFillRect(renderer, &indicator);
        }

        Color itemColor = (static_cast<int>(i) == currentSelection_) ? Color::YELLOW : Color::WHITE;
        SDL_SetRenderDrawColor(renderer, itemColor.r, itemColor.g, itemColor.b, itemColor.a);

        int textWidth = static_cast<int>(pauseMenuItems_[i].text.length() * 4);
        int textX = screenWidth / 2 - textWidth / 2;
        SDL_Rect textRect = {textX, itemY, textWidth, 6};
        SDL_RenderFillRect(renderer, &textRect);
    }
}

void GameStateManager::renderGameOver(SDL_Renderer* renderer, int screenWidth, int screenHeight) {
    // Dark overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 220);
    SDL_Rect overlay = {0, 0, screenWidth, screenHeight};
    SDL_RenderFillRect(renderer, &overlay);

    // "GAME OVER" title
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    int titleWidth = static_cast<int>(gameOverReason_.length() * 6);
    int titleX = screenWidth / 2 - titleWidth / 2;
    SDL_Rect titleRect = {titleX, screenHeight / 4, titleWidth, 8};
    SDL_RenderFillRect(renderer, &titleRect);

    // Score
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    int scoreY = screenHeight / 3;
    SDL_Rect scoreRect = {screenWidth / 2 - 30, scoreY, 60, 8};
    SDL_RenderFillRect(renderer, &scoreRect);

    // Menu items
    int menuY = screenHeight / 2;
    for (size_t i = 0; i < gameOverMenuItems_.size(); i++) {
        int itemY = menuY + static_cast<int>(i) * 20;

        if (static_cast<int>(i) == currentSelection_ && menuBlinkState_) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_Rect indicator = {screenWidth / 2 - 60, itemY, 4, 8};
            SDL_RenderFillRect(renderer, &indicator);
        }

        Color itemColor = (static_cast<int>(i) == currentSelection_) ? Color::YELLOW : Color::CYAN;
        SDL_SetRenderDrawColor(renderer, itemColor.r, itemColor.g, itemColor.b, itemColor.a);

        int textWidth = static_cast<int>(gameOverMenuItems_[i].text.length() * 4);
        int textX = screenWidth / 2 - textWidth / 2;
        SDL_Rect textRect = {textX, itemY, textWidth, 6};
        SDL_RenderFillRect(renderer, &textRect);
    }
}

void GameStateManager::handleMenuInput() {
    InputSystem& input = InputSystem::getInstance();

    // Get current menu
    std::vector<MenuItem>* currentMenu = nullptr;
    switch (currentState_) {
        case GameState::MENU:
            currentMenu = &mainMenuItems_;
            break;
        case GameState::PAUSED:
            currentMenu = &pauseMenuItems_;
            break;
        case GameState::GAME_OVER:
            currentMenu = &gameOverMenuItems_;
            break;
        default:
            return;
    }

    if (!currentMenu || currentMenu->empty()) return;

    // Navigation
    if (input.isButtonPressed(InputButton::ROTATE_LEFT)) {  // Up arrow
        moveSelection(-1);
    }
    if (input.isButtonPressed(InputButton::ROTATE_RIGHT)) {  // Down arrow
        moveSelection(1);
    }

    // Confirm selection
    if (input.isButtonPressed(InputButton::FIRE) ||
        input.isButtonPressed(InputButton::THRUST)) {
        executeMenuItem();
    }

    // Quick resume from pause
    if (currentState_ == GameState::PAUSED &&
        input.isButtonPressed(InputButton::PAUSE)) {
        if (resumeGameCallback_) resumeGameCallback_();
        transitionTo(GameState::PLAYING);
    }
}

void GameStateManager::executeMenuItem() {
    std::vector<MenuItem>* currentMenu = nullptr;

    switch (currentState_) {
        case GameState::MENU:
            currentMenu = &mainMenuItems_;
            break;
        case GameState::PAUSED:
            currentMenu = &pauseMenuItems_;
            break;
        case GameState::GAME_OVER:
            currentMenu = &gameOverMenuItems_;
            break;
        default:
            return;
    }

    if (currentMenu && currentSelection_ >= 0 &&
        currentSelection_ < static_cast<int>(currentMenu->size())) {
        MenuItem& item = (*currentMenu)[currentSelection_];
        if (item.enabled && item.action) {
            item.action();
        }
    }
}

void GameStateManager::moveSelection(int direction) {
    std::vector<MenuItem>* currentMenu = nullptr;

    switch (currentState_) {
        case GameState::MENU:
            currentMenu = &mainMenuItems_;
            break;
        case GameState::PAUSED:
            currentMenu = &pauseMenuItems_;
            break;
        case GameState::GAME_OVER:
            currentMenu = &gameOverMenuItems_;
            break;
        default:
            return;
    }

    if (!currentMenu || currentMenu->empty()) return;

    currentSelection_ += direction;

    // Wrap around
    if (currentSelection_ < 0) {
        currentSelection_ = static_cast<int>(currentMenu->size()) - 1;
    } else if (currentSelection_ >= static_cast<int>(currentMenu->size())) {
        currentSelection_ = 0;
    }

    // Skip disabled items
    int attempts = 0;
    while (!(*currentMenu)[currentSelection_].enabled &&
           attempts < static_cast<int>(currentMenu->size())) {
        currentSelection_ += direction;
        if (currentSelection_ < 0) {
            currentSelection_ = static_cast<int>(currentMenu->size()) - 1;
        } else if (currentSelection_ >= static_cast<int>(currentMenu->size())) {
            currentSelection_ = 0;
        }
        attempts++;
    }
}
