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
#include "../entities/Sinibomb.h"
#include "../entities/Warrior.h"
#include "../input/InputSystem.h"
#include <iostream>
#include <stdexcept>
#include <cstdio>
#include <algorithm>

GameEngine::GameEngine(int windowWidth, int windowHeight,
                       int logicalWidth, int logicalHeight)
    : window_(nullptr)
    , renderer_(nullptr)
    , running_(false)
    , logicalWidth_(logicalWidth)
    , logicalHeight_(logicalHeight)
    , enemySpawnTimer_(0.0f)
    , enemySpawnInterval_(2.0f)   // Spawn enemy every 2 seconds
    , maxEnemies_(10)             // Max 10 enemies at once
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
    for (Sinibomb* proj : projectiles_) {
        delete proj;
    }
    projectiles_.clear();

    for (Warrior* enemy : enemies_) {
        delete enemy;
    }
    enemies_.clear();

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

        // Check if player fired a weapon
        if (player_->isActive() && InputSystem::getInstance().isButtonPressed(InputButton::FIRE)) {
            if (player_->canFire()) {
                Sinibomb* bomb = player_->createSinibomb();
                if (bomb) {
                    projectiles_.push_back(bomb);
                    physicsSystem_->registerObject(bomb);
                }
            }
        }
    }

    // Update projectiles
    updateProjectiles(deltaTime);

    // Update enemies
    updateEnemies(deltaTime);

    // Spawn new enemies
    enemySpawnTimer_ += deltaTime;
    if (enemySpawnTimer_ >= enemySpawnInterval_ && enemies_.size() < static_cast<size_t>(maxEnemies_)) {
        spawnEnemy();
        enemySpawnTimer_ = 0.0f;
    }

    // Check collisions
    checkCollisions();

    // Clean up dead entities
    cleanupEntities();

    // Update task manager (will execute all active tasks)
    // This replicates the task execution from the executive loop
    taskManager_->update(deltaTime);

    // Update physics system
    physicsSystem_->update(deltaTime);

    // Update text renderer (timed messages)
    textRenderer_->update(deltaTime);
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

    // Render projectiles
    for (Sinibomb* proj : projectiles_) {
        if (proj && proj->isActive()) {
            proj->render(renderer_);
        }
    }

    // Render enemies
    for (Warrior* enemy : enemies_) {
        if (enemy && enemy->isActive()) {
            enemy->render(renderer_);
        }
    }

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

void GameEngine::updateProjectiles(float deltaTime) {
    for (Sinibomb* proj : projectiles_) {
        if (proj && proj->isActive()) {
            proj->update(deltaTime);
        }
    }
}

void GameEngine::updateEnemies(float deltaTime) {
    for (Warrior* enemy : enemies_) {
        if (enemy && enemy->isActive()) {
            // Set player as target
            if (player_ && player_->isActive()) {
                enemy->setTarget(player_.get());
            }
            enemy->update(deltaTime);
        }
    }
}

void GameEngine::checkCollisions() {
    // Projectile vs Enemy collisions
    for (Sinibomb* proj : projectiles_) {
        if (!proj || !proj->isActive()) continue;

        for (Warrior* enemy : enemies_) {
            if (!enemy || !enemy->isActive()) continue;

            // Circle-circle collision
            float dist = MathUtils::distance(proj->getPosition(), enemy->getPosition());
            float collisionDist = proj->getRadius() + enemy->getRadius();

            if (dist < collisionDist) {
                // Hit!
                enemy->takeDamage(proj->getDamage());
                proj->explode();
                break;
            }
        }
    }

    // Enemy vs Player collisions
    if (player_ && player_->isActive()) {
        for (Warrior* enemy : enemies_) {
            if (!enemy || !enemy->isActive()) continue;

            float dist = MathUtils::distance(player_->getPosition(), enemy->getPosition());
            float collisionDist = 8.0f + enemy->getRadius();  // Player radius ~8

            if (dist < collisionDist) {
                // Collision! Damage both
                enemy->takeDamage(0.5f);
                player_->takeDamage(0.3f);
            }
        }
    }
}

void GameEngine::spawnEnemy() {
    // Random spawn position at edge of screen
    Vector2 spawnPos;
    int edge = rand() % 4;

    switch (edge) {
        case 0: // Top
            spawnPos.x = static_cast<float>(rand() % logicalWidth_);
            spawnPos.y = 0.0f;
            break;
        case 1: // Right
            spawnPos.x = static_cast<float>(logicalWidth_);
            spawnPos.y = static_cast<float>(rand() % logicalHeight_);
            break;
        case 2: // Bottom
            spawnPos.x = static_cast<float>(rand() % logicalWidth_);
            spawnPos.y = static_cast<float>(logicalHeight_);
            break;
        case 3: // Left
            spawnPos.x = 0.0f;
            spawnPos.y = static_cast<float>(rand() % logicalHeight_);
            break;
    }

    // Random mission type
    WarriorMission mission = static_cast<WarriorMission>(rand() % static_cast<int>(WarriorMission::MINING));

    // Create enemy
    Warrior* enemy = new Warrior(mission);
    enemy->setPosition(spawnPos);

    // Set player as target
    if (player_) {
        enemy->setTarget(player_.get());
    }

    // Register with physics for screen wrapping
    physicsSystem_->registerObject(enemy);

    enemies_.push_back(enemy);
}

void GameEngine::cleanupEntities() {
    // Remove dead projectiles
    auto projIt = projectiles_.begin();
    while (projIt != projectiles_.end()) {
        if ((*projIt)->shouldRemove()) {
            physicsSystem_->unregisterObject(*projIt);
            delete *projIt;
            projIt = projectiles_.erase(projIt);
        } else {
            ++projIt;
        }
    }

    // Remove dead enemies
    auto enemyIt = enemies_.begin();
    while (enemyIt != enemies_.end()) {
        if ((*enemyIt)->shouldRemove()) {
            physicsSystem_->unregisterObject(*enemyIt);
            delete *enemyIt;
            enemyIt = enemies_.erase(enemyIt);
        } else {
            ++enemyIt;
        }
    }
}
