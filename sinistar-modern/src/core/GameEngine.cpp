/**
 * GameEngine.cpp
 *
 * Implementation of main game engine
 */

#include "GameEngine.h"
#include "TaskManager.h"
#include "Math.h"
#include "../systems/RenderSystem.h"
#include "../systems/PhysicsSystem.h"
#include "../graphics/TextRenderer.h"
#include "../graphics/Color.h"
#include "../entities/GameObject.h"
#include "../entities/Player.h"
#include "../input/InputSystem.h"
#include <iostream>
#include <stdexcept>
#include <cstdio>

GameEngine::GameEngine(int windowWidth, int windowHeight,
                       int logicalWidth, int logicalHeight)
    : window_(nullptr)
    , renderer_(nullptr)
    , running_(false)
    , logicalWidth_(logicalWidth)
    , logicalHeight_(logicalHeight)
    , lastFrameTime_(0)
{
    // Create window
    window_ = SDL_CreateWindow(
        "Sinistar",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );

    if (!window_) {
        throw std::runtime_error(std::string("Failed to create window: ") + SDL_GetError());
    }

    // Create renderer with VSync enabled (for 60 FPS cap)
    renderer_ = SDL_CreateRenderer(
        window_,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!renderer_) {
        SDL_DestroyWindow(window_);
        throw std::runtime_error(std::string("Failed to create renderer: ") + SDL_GetError());
    }

    // Set logical resolution (maintains aspect ratio)
    SDL_RenderSetLogicalSize(renderer_, logicalWidth_, logicalHeight_);

    // Set scale quality to nearest neighbor (for pixel-perfect arcade feel)
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");

    std::cout << "Window created: " << windowWidth << "x" << windowHeight << std::endl;
    std::cout << "Logical resolution: " << logicalWidth << "x" << logicalHeight << std::endl;

    // Initialize game systems
    taskManager_ = std::make_unique<TaskManager>();
    renderSystem_ = std::make_unique<RenderSystem>(renderer_);
    physicsSystem_ = std::make_unique<PhysicsSystem>(
        static_cast<float>(logicalWidth_),
        static_cast<float>(logicalHeight_)
    );
    textRenderer_ = std::make_unique<TextRenderer>(renderer_);

    // Initialize Phase 3 player
    initPhase3Player();

    std::cout << "Game engine initialized" << std::endl;
    std::cout << "Phase 3: Player ship ready!" << std::endl;
}

GameEngine::~GameEngine() {
    // Clean up entities
    player_.reset();

    // Clean up systems
    textRenderer_.reset();
    physicsSystem_.reset();
    renderSystem_.reset();
    taskManager_.reset();

    // Clean up SDL resources
    if (renderer_) {
        SDL_DestroyRenderer(renderer_);
    }
    if (window_) {
        SDL_DestroyWindow(window_);
    }

    std::cout << "Game engine destroyed" << std::endl;
}

void GameEngine::run() {
    running_ = true;
    lastFrameTime_ = SDL_GetPerformanceCounter();

    std::cout << "Starting game loop (60 FPS target)..." << std::endl;

    // Main game loop - replicates executive loop from EXEC.SRC
    while (running_) {
        // Calculate delta time
        Uint64 currentTime = SDL_GetPerformanceCounter();
        float deltaTime = static_cast<float>(currentTime - lastFrameTime_) /
                         static_cast<float>(SDL_GetPerformanceFrequency());
        lastFrameTime_ = currentTime;

        // Cap delta time to prevent spiral of death
        if (deltaTime > 0.25f) {
            deltaTime = FRAME_TIME;
        }

        // Process input and events
        handleEvents();

        // Update game state
        update(deltaTime);

        // Render frame
        render();
    }

    std::cout << "Game loop ended" << std::endl;
}

void GameEngine::quit() {
    running_ = false;
}

void GameEngine::handleEvents() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        // Let input system process event first
        InputSystem::getInstance().processEvent(event);

        switch (event.type) {
            case SDL_QUIT:
                quit();
                break;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    std::cout << "Window resized: " << event.window.data1
                              << "x" << event.window.data2 << std::endl;
                }
                break;

            default:
                break;
        }
    }

    // Check for quit button (ESC or gamepad back)
    if (InputSystem::getInstance().isButtonPressed(InputButton::QUIT)) {
        quit();
    }
}

void GameEngine::update(float deltaTime) {
    // Update input system
    InputSystem::getInstance().update();

    // Update player
    if (player_) {
        player_->handleInput(deltaTime);
        player_->update(deltaTime);
    }

    // Update task manager (will execute all active tasks)
    // This replicates the task execution from the executive loop
    taskManager_->update(deltaTime);

    // Update physics system
    physicsSystem_->update(deltaTime);

    // Update text renderer (timed messages)
    textRenderer_->update(deltaTime);

    // TODO: Add other system updates here:
    // - Collision detection
    // - AI
    // - Audio
}

void GameEngine::render() {
    // Clear screen (black background like original arcade)
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    // Render game objects via render system
    renderSystem_->render();

    // Draw white border around the screen
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    SDL_Rect border = {0, 0, logicalWidth_, logicalHeight_};
    SDL_RenderDrawRect(renderer_, &border);

    // Render player
    if (player_ && player_->isActive()) {
        player_->render(renderer_);
    }

    // Render text/UI
    textRenderer_->render();

    // Draw HUD
    drawHUD();

    // Present frame
    SDL_RenderPresent(renderer_);
}

void GameEngine::drawHUD() {
    if (!player_) return;

    // Draw player stats in top-left
    char buffer[64];

    // Lives
    snprintf(buffer, sizeof(buffer), "LIVES: %d", player_->getLives());
    textRenderer_->drawText(buffer, 10, 10, BitmapFont::Size::SMALL_3x5, Color::WHITE);

    // Bombs
    snprintf(buffer, sizeof(buffer), "BOMBS: %d", player_->getBombCount());
    textRenderer_->drawText(buffer, 10, 20, BitmapFont::Size::SMALL_3x5, Color::YELLOW);

    // Shield
    int shieldPercent = static_cast<int>(player_->getShieldStrength() * 100);
    snprintf(buffer, sizeof(buffer), "SHIELD: %d%%", shieldPercent);
    Color shieldColor = player_->hasShield() ? Color::GREEN : Color::RED;
    textRenderer_->drawText(buffer, 10, 30, BitmapFont::Size::SMALL_3x5, shieldColor);
}

void GameEngine::initPhase3Player() {
    std::cout << "\n=== Phase 3 Player Initialization ===" << std::endl;

    // Initialize color palette
    ColorPalette::getInstance().initializeDefaultPalette();
    std::cout << "Color palette initialized" << std::endl;

    // Create player ship
    player_ = std::make_unique<Player>();

    // Position player in center of screen
    player_->setPosition(
        logicalWidth_ / 2.0f,
        logicalHeight_ / 2.0f
    );

    // Give player some starting bombs for testing
    player_->addBombs(10);

    // Register player with physics system (for screen wrapping)
    physicsSystem_->registerObject(player_.get());

    std::cout << "Player ship created at center" << std::endl;

    // Add welcome message
    textRenderer_->showMessage(
        "SINISTAR - PHASE 3",
        Vector2(logicalWidth_ / 2.0f, 50.0f),
        5.0f,  // Show for 5 seconds
        Color::WHITE,
        BitmapFont::Size::LARGE_6x8
    );

    textRenderer_->showMessage(
        "PLAYER SHIP READY",
        Vector2(logicalWidth_ / 2.0f, 62.0f),
        5.0f,
        Color::CYAN,
        BitmapFont::Size::SMALL_3x5
    );

    // Controls hint
    textRenderer_->showMessage(
        "ARROWS: ROTATE  UP: THRUST  SPACE: FIRE  ESC: QUIT",
        Vector2(logicalWidth_ / 2.0f, logicalHeight_ - 15.0f),
        0.0f,  // Permanent
        Color::YELLOW,
        BitmapFont::Size::SMALL_3x5
    );

    std::cout << "=== Phase 3 Player Ready ===" << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  Arrow Keys / WASD - Rotate & Thrust" << std::endl;
    std::cout << "  Space / Ctrl - Fire weapon" << std::endl;
    std::cout << "  ESC - Quit" << std::endl;
    std::cout << "  Gamepad also supported!" << std::endl;
    std::cout << std::endl;
}
