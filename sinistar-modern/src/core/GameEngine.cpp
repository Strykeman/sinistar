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
#include "../entities/Crystal.h"
#include "../entities/Worker.h"
#include "../entities/Sinistar.h"
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
    , crystalSpawnTimer_(0.0f)
    , crystalSpawnInterval_(5.0f) // Spawn crystal every 5 seconds
    , maxCrystals_(15)            // Max 15 crystals at once
    , workerSpawnTimer_(0.0f)
    , workerSpawnInterval_(3.0f)  // Spawn worker every 3 seconds
    , maxWorkers_(8)              // Max 8 workers at once
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

    for (Crystal* crystal : crystals_) {
        delete crystal;
    }
    crystals_.clear();

    for (Worker* worker : workers_) {
        delete worker;
    }
    workers_.clear();

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

        // Update crystals
        updateCrystals(deltaTime);

        // Update workers
        updateWorkers(deltaTime);

        // Update Sinistar
        if (sinistar_) {
            sinistar_->update(deltaTime);

            // Display voice lines
            std::string voiceLine = sinistar_->getVoiceLine();
            if (!voiceLine.empty()) {
                textRenderer_->showMessage(
                    voiceLine,
                    Vector2(logicalWidth_ / 2.0f, 50.0f),
                    3.0f,
                    Color::RED,
                    BitmapFont::Size::LARGE_6x8
                );
                sinistar_->clearVoiceLine();
            }
        }

        // Spawn new enemies
        enemySpawnTimer_ += deltaTime;
        if (enemySpawnTimer_ >= enemySpawnInterval_ && enemies_.size() < static_cast<size_t>(maxEnemies_)) {
            spawnEnemy();
            enemySpawnTimer_ = 0.0f;
        }

        // Spawn new crystals
        crystalSpawnTimer_ += deltaTime;
        if (crystalSpawnTimer_ >= crystalSpawnInterval_ && crystals_.size() < static_cast<size_t>(maxCrystals_)) {
            spawnCrystal();
            crystalSpawnTimer_ = 0.0f;
        }

        // Spawn new workers
        workerSpawnTimer_ += deltaTime;
        if (workerSpawnTimer_ >= workerSpawnInterval_ && workers_.size() < static_cast<size_t>(maxWorkers_)) {
            spawnWorker();
            workerSpawnTimer_ = 0.0f;
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

    // Render crystals (background objects)
    for (Crystal* crystal : crystals_) {
        if (crystal && crystal->isActive()) {
            crystal->render(renderer_);
        }
    }

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

    // Render workers
    for (Worker* worker : workers_) {
        if (worker && worker->isActive()) {
            worker->render(renderer_);
        }
    }

    // Render Sinistar (before player so player is always visible)
    if (sinistar_) {
        sinistar_->render(renderer_);
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

    // Draw Sinistar construction progress (center top)
    if (sinistar_ && !sinistar_->isActive()) {
        float progress = sinistar_->getConstructionProgress();
        int progressPercent = static_cast<int>(progress * 100);
        snprintf(buffer, sizeof(buffer), "SINISTAR: %d%%", progressPercent);
        Color progressColor = progress < 0.5f ? Color::YELLOW : Color::RED;
        textRenderer_->drawText(buffer, logicalWidth_ / 2 - 40, 10, BitmapFont::Size::SMALL_3x5, progressColor);

        // Progress bar
        int barWidth = static_cast<int>(80 * progress);
        SDL_SetRenderDrawColor(renderer_, progressColor.r, progressColor.g, progressColor.b, 255);
        SDL_Rect progressBar = {logicalWidth_ / 2 - 40, 18, barWidth, 3};
        SDL_RenderFillRect(renderer_, &progressBar);
    }

    // Draw Sinistar health if active (center top)
    if (sinistar_ && sinistar_->isActive()) {
        float healthRatio = sinistar_->getHealth() / sinistar_->getMaxHealth();
        int healthPercent = static_cast<int>(healthRatio * 100);
        snprintf(buffer, sizeof(buffer), "SINISTAR HP: %d%%", healthPercent);
        Color healthColor = healthRatio > 0.5f ? Color::RED : Color::YELLOW;
        textRenderer_->drawText(buffer, logicalWidth_ / 2 - 50, 10, BitmapFont::Size::SMALL_3x5, healthColor);

        // Health bar
        int barWidth = static_cast<int>(100 * healthRatio);
        SDL_SetRenderDrawColor(renderer_, healthColor.r, healthColor.g, healthColor.b, 255);
        SDL_Rect healthBar = {logicalWidth_ / 2 - 50, 18, barWidth, 3};
        SDL_RenderFillRect(renderer_, &healthBar);
    }
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

    // Create Sinistar (starts in BUILDING state)
    sinistar_ = std::make_unique<Sinistar>();
    sinistar_->setTarget(player_.get());
    physicsSystem_->registerObject(sinistar_.get());
    std::cout << "Sinistar created (under construction)" << std::endl;

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

    for (Crystal* crystal : crystals_) {
        physicsSystem_->unregisterObject(crystal);
        delete crystal;
    }
    crystals_.clear();

    for (Worker* worker : workers_) {
        physicsSystem_->unregisterObject(worker);
        delete worker;
    }
    workers_.clear();

    // Clear particles
    particleSystem_->clear();

    // Reset score manager
    scoreManager_->reset();

    // Reset spawn parameters to level 1
    enemySpawnTimer_ = 0.0f;
    enemySpawnInterval_ = scoreManager_->getEnemySpawnRate();
    maxEnemies_ = scoreManager_->getMaxEnemies();

    crystalSpawnTimer_ = 0.0f;
    crystalSpawnInterval_ = 5.0f;
    maxCrystals_ = 15;

    workerSpawnTimer_ = 0.0f;
    workerSpawnInterval_ = 3.0f;
    maxWorkers_ = 8;

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

    // Reset Sinistar
    if (sinistar_) {
        physicsSystem_->unregisterObject(sinistar_.get());
    }
    sinistar_ = std::make_unique<Sinistar>();
    sinistar_->setTarget(player_.get());
    physicsSystem_->registerObject(sinistar_.get());

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

        // Enemy Worker collisions
        for (Worker* worker : workers_) {
            if (!worker || !worker->isActive()) continue;

            float dist = MathUtils::distance(player_->getPosition(), worker->getPosition());
            float collisionDist = 8.0f + worker->getRadius();  // Player radius ~8

            if (dist < collisionDist) {
                // Collision with worker - less damage than warrior
                Vector2 impactDir = (worker->getPosition() - player_->getPosition()).normalized();

                bool workerDestroyed = worker->takeDamage(0.5f);
                if (workerDestroyed) {
                    particleSystem_->createExplosion(worker->getPosition(), {100, 100, 255, 255}, 20);
                    AudioManager::getInstance().playSound(SoundEffect::ENEMY_EXPLODE);

                    // Bonus if worker was carrying crystal
                    if (worker->isCarryingCrystal()) {
                        scoreManager_->addScore(150);
                    } else {
                        scoreManager_->addScore(50);
                    }
                    scoreManager_->addKill();
                }

                bool playerDestroyed = player_->takeDamage(0.2f);  // Less damage from worker
                if (playerDestroyed) {
                    particleSystem_->createExplosion(player_->getPosition(), {0, 255, 255, 255}, 40);
                    AudioManager::getInstance().playSound(SoundEffect::PLAYER_DIE);
                } else {
                    particleSystem_->createImpact(player_->getPosition(), impactDir * -1.0f, {255, 255, 255, 255});
                    AudioManager::getInstance().playSound(SoundEffect::PLAYER_HIT);
                }
            }
        }
    }

    // Projectile vs Crystal collisions
    for (Sinibomb* proj : projectiles_) {
        if (!proj || !proj->isActive()) continue;

        for (Crystal* crystal : crystals_) {
            if (!crystal || !crystal->isActive()) continue;

            float dist = MathUtils::distance(proj->getPosition(), crystal->getPosition());
            float collisionDist = proj->getRadius() + crystal->getRadius();

            if (dist < collisionDist) {
                // Hit crystal!
                bool destroyed = crystal->takeDamage(proj->getDamage());

                if (destroyed) {
                    // Crystal destroyed - award points based on size
                    int points = static_cast<int>(crystal->getCrystalAmount() * 25.0f);
                    scoreManager_->addScore(points);

                    // Green explosion
                    particleSystem_->createExplosion(crystal->getPosition(), {100, 255, 100, 255}, 25);
                    AudioManager::getInstance().playSound(SoundEffect::PICKUP_CRYSTAL, 0.8f);
                } else {
                    // Crystal hit but not destroyed
                    particleSystem_->createImpact(crystal->getPosition(), proj->getVelocity(), {150, 255, 150, 255});
                }

                // Projectile explodes
                proj->explode();
                scoreManager_->recordShotHit();
                break;
            }
        }
    }

    // Projectile vs Worker collisions
    for (Sinibomb* proj : projectiles_) {
        if (!proj || !proj->isActive()) continue;

        for (Worker* worker : workers_) {
            if (!worker || !worker->isActive()) continue;

            float dist = MathUtils::distance(proj->getPosition(), worker->getPosition());
            float collisionDist = proj->getRadius() + worker->getRadius();

            if (dist < collisionDist) {
                // Hit worker!
                bool destroyed = worker->takeDamage(proj->getDamage());

                if (destroyed) {
                    // Worker destroyed
                    particleSystem_->createExplosion(worker->getPosition(), {100, 100, 255, 255}, 25);
                    AudioManager::getInstance().playSound(SoundEffect::ENEMY_EXPLODE);

                    // Award points (bonus if carrying crystal)
                    int basePoints = 50;
                    if (worker->isCarryingCrystal()) {
                        basePoints = 150;
                    }
                    int points = static_cast<int>(basePoints * scoreManager_->getDifficultyMultiplier());
                    scoreManager_->addScore(points);
                    scoreManager_->addKill();
                } else {
                    // Worker hit but not destroyed
                    particleSystem_->createImpact(worker->getPosition(), proj->getVelocity(), {150, 150, 255, 255});
                    AudioManager::getInstance().playSound(SoundEffect::ENEMY_HIT, 0.5f);
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

    // Projectile vs Sinistar collisions
    if (sinistar_ && sinistar_->isActive()) {
        for (Sinibomb* proj : projectiles_) {
            if (!proj || !proj->isActive()) continue;

            float dist = MathUtils::distance(proj->getPosition(), sinistar_->getPosition());
            float collisionDist = proj->getRadius() + sinistar_->getRadius();

            if (dist < collisionDist) {
                // Hit Sinistar!
                bool destroyed = sinistar_->takeDamage(proj->getDamage());

                if (destroyed) {
                    // Sinistar destroyed! Huge points!
                    int points = 10000 * scoreManager_->getDifficultyMultiplier();
                    scoreManager_->addScore(points);
                    scoreManager_->addKill();

                    // Massive explosion
                    particleSystem_->createExplosion(sinistar_->getPosition(), {255, 100, 0, 255}, 60);
                    AudioManager::getInstance().playSound(SoundEffect::ENEMY_EXPLODE);

                    // Victory message
                    textRenderer_->showMessage(
                        "SINISTAR DESTROYED! +10000",
                        Vector2(logicalWidth_ / 2.0f, logicalHeight_ / 2.0f),
                        5.0f,
                        Color::YELLOW,
                        BitmapFont::Size::LARGE_6x8
                    );
                } else {
                    // Hit but not destroyed
                    particleSystem_->createImpact(sinistar_->getPosition(), proj->getVelocity(), {255, 200, 0, 255});
                    AudioManager::getInstance().playSound(SoundEffect::ENEMY_HIT, 0.8f);
                    scoreManager_->addScore(50);
                }

                // Projectile explodes
                proj->explode();
                scoreManager_->recordShotHit();
                break;
            }
        }

        // Sinistar bite attack vs Player
        if (player_ && player_->isActive()) {
            if (sinistar_->getState() == SinistarState::BITING) {
                float dist = MathUtils::distance(player_->getPosition(), sinistar_->getPosition());
                float biteRange = sinistar_->getRadius() + 10.0f;

                if (dist < biteRange) {
                    // Sinistar bites player!
                    bool playerDestroyed = player_->takeDamage(1.0f);  // Heavy damage!

                    if (playerDestroyed) {
                        particleSystem_->createExplosion(player_->getPosition(), {0, 255, 255, 255}, 50);
                        AudioManager::getInstance().playSound(SoundEffect::PLAYER_DIE);
                    } else {
                        particleSystem_->createImpact(player_->getPosition(),
                            (player_->getPosition() - sinistar_->getPosition()).normalized(),
                            {255, 255, 255, 255});
                        AudioManager::getInstance().playSound(SoundEffect::PLAYER_HIT);
                    }
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

void GameEngine::spawnCrystal() {
    // Random position in play area (not at edges)
    Vector2 spawnPos;
    spawnPos.x = 40.0f + static_cast<float>(rand() % (logicalWidth_ - 80));
    spawnPos.y = 40.0f + static_cast<float>(rand() % (logicalHeight_ - 80));

    // Create crystal
    Crystal* crystal = new Crystal();
    crystal->setPosition(spawnPos);

    // Register with physics for screen wrapping
    physicsSystem_->registerObject(crystal);

    crystals_.push_back(crystal);
}

void GameEngine::spawnWorker() {
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

    // Create worker
    Worker* worker = new Worker();
    worker->setPosition(spawnPos);

    // Set player reference
    if (player_) {
        worker->setPlayer(player_.get());
    }

    // Assign nearest crystal as target
    if (!crystals_.empty()) {
        Crystal* nearest = nullptr;
        float nearestDist = 999999.0f;

        for (Crystal* crystal : crystals_) {
            if (crystal && crystal->isActive() && !crystal->isDepleted()) {
                float dist = MathUtils::distance(spawnPos, crystal->getPosition());
                if (dist < nearestDist) {
                    nearestDist = dist;
                    nearest = crystal;
                }
            }
        }

        if (nearest) {
            worker->setTargetCrystal(nearest);
        }
    }

    // Register with physics for screen wrapping
    physicsSystem_->registerObject(worker);

    workers_.push_back(worker);
}

void GameEngine::updateCrystals(float deltaTime) {
    for (Crystal* crystal : crystals_) {
        if (crystal && crystal->isActive()) {
            crystal->update(deltaTime);
        }
    }
}

void GameEngine::updateWorkers(float deltaTime) {
    for (Worker* worker : workers_) {
        if (worker && worker->isActive()) {
            // Check if worker is returning and near Sinistar
            if (worker->getState() == WorkerState::RETURNING && worker->isCarryingCrystal()) {
                float distToCenter = MathUtils::distance(worker->getPosition(), Vector2(128.0f, 122.0f));
                if (distToCenter < 30.0f && sinistar_) {
                    // Deliver crystal to Sinistar!
                    float amount = worker->getCrystalAmount();
                    bool completed = sinistar_->addCrystal(amount);
                    worker->dropCrystal();  // Worker delivers crystal

                    if (completed && sinistar_->isConstructed()) {
                        // Sinistar construction complete!
                        sinistar_->activate();
                        textRenderer_->showMessage(
                            "BEWARE, I LIVE!",
                            Vector2(logicalWidth_ / 2.0f, 60.0f),
                            3.0f,
                            Color::RED,
                            BitmapFont::Size::LARGE_6x8
                        );
                        AudioManager::getInstance().playSound(SoundEffect::ENEMY_EXPLODE);  // Use explosion for dramatic effect
                    }
                }
            }

            worker->update(deltaTime);

            // Reassign crystal target if needed
            if (worker->getState() == WorkerState::SEARCHING) {
                Crystal* nearest = nullptr;
                float nearestDist = 999999.0f;

                for (Crystal* crystal : crystals_) {
                    if (crystal && crystal->isActive() && !crystal->isDepleted()) {
                        float dist = MathUtils::distance(worker->getPosition(), crystal->getPosition());
                        if (dist < nearestDist) {
                            nearestDist = dist;
                            nearest = crystal;
                        }
                    }
                }

                if (nearest) {
                    worker->setTargetCrystal(nearest);
                }
            }
        }
    }
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

    // Remove depleted crystals
    auto crystalIt = crystals_.begin();
    while (crystalIt != crystals_.end()) {
        if ((*crystalIt)->shouldRemove()) {
            physicsSystem_->unregisterObject(*crystalIt);
            delete *crystalIt;
            crystalIt = crystals_.erase(crystalIt);
        } else {
            ++crystalIt;
        }
    }

    // Remove dead workers
    auto workerIt = workers_.begin();
    while (workerIt != workers_.end()) {
        if ((*workerIt)->shouldRemove()) {
            // Drop crystal if carrying
            if ((*workerIt)->isCarryingCrystal()) {
                // Could spawn pickup item here
                scoreManager_->addScore(50);  // Bonus for destroying worker with crystal
            }
            physicsSystem_->unregisterObject(*workerIt);
            delete *workerIt;
            workerIt = workers_.erase(workerIt);
        } else {
            ++workerIt;
        }
    }
}
