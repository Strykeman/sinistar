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
#include <iostream>
#include <stdexcept>
#include <vector>

// Test objects for Phase 2
static std::vector<GameObject*> testObjects;

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

    // Initialize Phase 2 test scene
    initPhase2Test();

    std::cout << "Game engine initialized" << std::endl;
    std::cout << "Phase 2 systems active: Math, Physics, Text Rendering, Color Palette" << std::endl;
}

GameEngine::~GameEngine() {
    // Clean up test objects
    for (GameObject* obj : testObjects) {
        delete obj;
    }
    testObjects.clear();

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
        switch (event.type) {
            case SDL_QUIT:
                quit();
                break;

            case SDL_KEYDOWN:
                // ESC to quit (temporary - will be removed later)
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    quit();
                }
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
}

void GameEngine::update(float deltaTime) {
    // Update task manager (will execute all active tasks)
    // This replicates the task execution from the executive loop
    taskManager_->update(deltaTime);

    // Update physics system (Phase 2)
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

    // Phase 2 Test: Render test objects
    for (GameObject* obj : testObjects) {
        if (obj && obj->isActive()) {
            Vector2 pos = obj->getPosition();

            // Draw object as a colored circle (using rectangles)
            SDL_Rect objRect = {
                static_cast<int>(pos.x - 2),
                static_cast<int>(pos.y - 2),
                4,
                4
            };

            // Color based on status (test)
            uint32_t status = obj->getStatus();
            Color objColor = ColorPalette::getInstance().getColor(status % 16);
            SDL_SetRenderDrawColor(renderer_, objColor.r, objColor.g, objColor.b, objColor.a);
            SDL_RenderFillRect(renderer_, &objRect);

            // Draw velocity vector
            Vector2 vel = obj->getVelocity();
            if (vel.lengthSquared() > 0.1f) {
                SDL_SetRenderDrawColor(renderer_, 0, 255, 0, 128);
                SDL_RenderDrawLine(renderer_,
                    static_cast<int>(pos.x),
                    static_cast<int>(pos.y),
                    static_cast<int>(pos.x + vel.x * 2.0f),
                    static_cast<int>(pos.y + vel.y * 2.0f)
                );
            }
        }
    }

    // Render text (Phase 2 test)
    textRenderer_->render();

    // Present frame
    SDL_RenderPresent(renderer_);
}

void GameEngine::initPhase2Test() {
    std::cout << "\n=== Phase 2 Test Initialization ===" << std::endl;

    // Initialize color palette
    ColorPalette::getInstance().initializeDefaultPalette();
    std::cout << "Color palette initialized" << std::endl;

    // Create test objects with different velocities and positions
    for (int i = 0; i < 10; i++) {
        GameObject* obj = new GameObject();

        // Random-ish position
        float x = (logicalWidth_ / 11.0f) * (i + 1);
        float y = logicalHeight_ / 2.0f + (i % 3 - 1) * 30.0f;
        obj->setPosition(x, y);

        // Random-ish velocity
        float vx = (i % 5 - 2) * 10.0f;
        float vy = ((i + 3) % 5 - 2) * 10.0f;
        obj->setVelocity(vx, vy);

        // Set status for color
        obj->setStatus(i + 2);  // Use palette colors 2-11

        // Register with physics system
        physicsSystem_->registerObject(obj);

        testObjects.push_back(obj);
    }
    std::cout << "Created " << testObjects.size() << " test objects with physics" << std::endl;

    // Add test messages
    textRenderer_->showMessage(
        "SINISTAR - PHASE 2",
        Vector2(logicalWidth_ / 2.0f, 20.0f),
        0.0f,  // Permanent
        Color::WHITE,
        BitmapFont::Size::LARGE_6x8
    );

    textRenderer_->showMessage(
        "MATH + PHYSICS + TEXT RENDERING",
        Vector2(logicalWidth_ / 2.0f, 32.0f),
        0.0f,
        Color::CYAN,
        BitmapFont::Size::SMALL_3x5
    );

    textRenderer_->showMessage(
        "SCREEN WRAPPING ACTIVE",
        Vector2(logicalWidth_ / 2.0f, logicalHeight_ - 20.0f),
        0.0f,
        Color::YELLOW,
        BitmapFont::Size::SMALL_3x5
    );

    std::cout << "Test messages added" << std::endl;
    std::cout << "=== Phase 2 Test Ready ===" << std::endl;
    std::cout << "Watch objects move and wrap around screen!" << std::endl;
    std::cout << "Press ESC to quit\n" << std::endl;
}
