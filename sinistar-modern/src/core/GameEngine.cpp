/**
 * GameEngine.cpp
 *
 * Implementation of main game engine
 */

#include "GameEngine.h"
#include "TaskManager.h"
#include "GameState.h"
#include "ScoreManager.h"
#include "Math.h"
#include "../systems/RenderSystem.h"
#include "../systems/PhysicsSystem.h"
#include "../graphics/TextRenderer.h"
#include "../graphics/Particle.h"
#include "../graphics/Color.h"
#include "../audio/AudioManager.h"
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
    particleSystem_ = std::make_unique<ParticleSystem>();
    stateManager_ = std::make_unique<GameStateManager>();
    scoreManager_ = std::make_unique<ScoreManager>();

    // Initialize audio system
    AudioManager::getInstance().initialize();

    // Setup state callbacks
    stateManager_->setStartGameCallback([this]() { resetGame(); });
    stateManager_->setResumeGameCallback([this]() { /* Resume game */ });
    stateManager_->setQuitCallback([this]() { quit(); });

    // Initialize game
    initializeGame();

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
    AudioManager::getInstance().shutdown();
    scoreManager_.reset();
    stateManager_.reset();
    particleSystem_.reset();
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

    // Let state manager handle input
    stateManager_->handleInput();
}

void GameEngine::update(float deltaTime) {
    // Update input system
    InputSystem::getInstance().update();

    // Update state manager
    stateManager_->update(deltaTime);

    // Check for quit state
    if (stateManager_->getState() == GameState::QUIT) {
        quit();
        return;
    }

    // Only update game when playing
    if (stateManager_->isPlaying()) {
        // Update score manager
        scoreManager_->update(deltaTime);

        // Check for level advancement
        if (scoreManager_->shouldAdvanceLevel()) {
            scoreManager_->advanceLevel();

            // Update difficulty based on new level
            enemySpawnInterval_ = scoreManager_->getEnemySpawnRate();
            maxEnemies_ = scoreManager_->getMaxEnemies();

            // Show level up message
            char levelMsg[64];
            snprintf(levelMsg, sizeof(levelMsg), "LEVEL %d", scoreManager_->getLevel());
            textRenderer_->showMessage(
                levelMsg,
                Vector2(logicalWidth_ / 2.0f, logicalHeight_ / 2.0f),
                3.0f,
                Color::YELLOW,
                BitmapFont::Size::LARGE_6x8
            );
        }
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
                        // Play shoot sound and track stats
                        AudioManager::getInstance().playSound(SoundEffect::PLAYER_SHOOT, 0.6f);
                        scoreManager_->recordShotFired();
                    }
                }
            }

            // Check for game over
            if (!player_->isActive() || player_->getLives() <= 0) {
                stateManager_->setGameOverReason("GAME OVER");
                stateManager_->setFinalScore(scoreManager_->getScore());
                stateManager_->transitionTo(GameState::GAME_OVER);

                // Check for high score
                if (scoreManager_->isHighScore(scoreManager_->getScore())) {
                    scoreManager_->addHighScore("YOU", scoreManager_->getScore(), scoreManager_->getLevel());
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
    }

    // Always update particle system (even when paused for fade effects)
    particleSystem_->update(deltaTime);

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

    // Render particles (behind other objects)
    particleSystem_->render(renderer_);

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

    // Draw HUD (only during gameplay)
    if (stateManager_->isPlaying()) {
        drawHUD();
    }

    // Render state overlay (menu, pause, game over)
    stateManager_->render(renderer_, logicalWidth_, logicalHeight_);

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

    // Draw score and level in top-right
    snprintf(buffer, sizeof(buffer), "SCORE: %d", scoreManager_->getScore());
    textRenderer_->drawText(buffer, logicalWidth_ - 100, 10, BitmapFont::Size::SMALL_3x5, Color::CYAN);

    snprintf(buffer, sizeof(buffer), "LEVEL: %d", scoreManager_->getLevel());
    textRenderer_->drawText(buffer, logicalWidth_ - 100, 20, BitmapFont::Size::SMALL_3x5, Color::MAGENTA);

    snprintf(buffer, sizeof(buffer), "KILLS: %d", scoreManager_->getKillCount());
    textRenderer_->drawText(buffer, logicalWidth_ - 100, 30, BitmapFont::Size::SMALL_3x5, Color::WHITE);
}

void GameEngine::initializeGame() {
    std::cout << "\n=== Game Initialization ===" << std::endl;

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

    // Give player some starting bombs
    player_->addBombs(10);

    // Register player with physics system (for screen wrapping)
    physicsSystem_->registerObject(player_.get());

    std::cout << "Player ship created at center" << std::endl;

    // Controls hint
    textRenderer_->showMessage(
        "ARROWS: ROTATE  UP: THRUST  SPACE: FIRE  P: PAUSE  ESC: QUIT",
        Vector2(logicalWidth_ / 2.0f, logicalHeight_ - 15.0f),
        0.0f,  // Permanent
        Color::YELLOW,
        BitmapFont::Size::SMALL_3x5
    );

    std::cout << "=== Game Initialized ===" << std::endl;
}

void GameEngine::resetGame() {
    std::cout << "Resetting game..." << std::endl;

    // Clear all entities
    for (Sinibomb* proj : projectiles_) {
        physicsSystem_->unregisterObject(proj);
        delete proj;
    }
    projectiles_.clear();

    for (Warrior* enemy : enemies_) {
        physicsSystem_->unregisterObject(enemy);
        delete enemy;
    }
    enemies_.clear();

    // Clear particles
    particleSystem_->clear();

    // Reset score manager
    scoreManager_->reset();

    // Reset spawn parameters to level 1
    enemySpawnTimer_ = 0.0f;
    enemySpawnInterval_ = scoreManager_->getEnemySpawnRate();
    maxEnemies_ = scoreManager_->getMaxEnemies();

    // Reset player
    if (player_) {
        physicsSystem_->unregisterObject(player_.get());
    }

    player_ = std::make_unique<Player>();
    player_->setPosition(logicalWidth_ / 2.0f, logicalHeight_ / 2.0f);
    player_->setVelocity(0.0f, 0.0f);
    player_->setRotation(0.0f);
    player_->addBombs(10);
    player_->setLives(3);
    physicsSystem_->registerObject(player_.get());

    std::cout << "Game reset complete" << std::endl;
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
                bool destroyed = enemy->takeDamage(proj->getDamage());

                // Trigger effects
                if (destroyed) {
                    // Enemy destroyed - big explosion
                    particleSystem_->createExplosion(enemy->getPosition(), {255, 100, 0, 255}, 30);
                    AudioManager::getInstance().playSound(SoundEffect::ENEMY_EXPLODE);

                    // Award points
                    int basePoints = 100;
                    int points = static_cast<int>(basePoints * scoreManager_->getDifficultyMultiplier());
                    scoreManager_->addScore(points);
                    scoreManager_->addKill();
                } else {
                    // Enemy hit but not destroyed - small impact
                    particleSystem_->createImpact(enemy->getPosition(), proj->getVelocity(), {255, 200, 0, 255});
                    AudioManager::getInstance().playSound(SoundEffect::ENEMY_HIT, 0.5f);

                    // Small points for hit
                    scoreManager_->addScore(10);
                }

                // Projectile explodes
                particleSystem_->createExplosion(proj->getPosition(), {255, 255, 0, 255}, 15);
                AudioManager::getInstance().playSound(SoundEffect::SINIBOMB_EXPLODE, 0.7f);
                proj->explode();
                scoreManager_->recordShotHit();
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
                Vector2 impactDir = (enemy->getPosition() - player_->getPosition()).normalized();

                bool enemyDestroyed = enemy->takeDamage(0.5f);
                if (enemyDestroyed) {
                    particleSystem_->createExplosion(enemy->getPosition(), {255, 0, 0, 255}, 25);
                    AudioManager::getInstance().playSound(SoundEffect::ENEMY_EXPLODE);
                }

                bool playerDestroyed = player_->takeDamage(0.3f);
                if (playerDestroyed) {
                    particleSystem_->createExplosion(player_->getPosition(), {0, 255, 255, 255}, 40);
                    AudioManager::getInstance().playSound(SoundEffect::PLAYER_DIE);
                } else {
                    // Player hit but survived
                    particleSystem_->createImpact(player_->getPosition(), impactDir * -1.0f, {255, 255, 255, 255});
                    AudioManager::getInstance().playSound(SoundEffect::PLAYER_HIT);
                }
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
