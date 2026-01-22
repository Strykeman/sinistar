/**
 * GameEngine.cpp
 *
 * Implementation of main game engine
 */

#include "GameEngine.h"
#include "TaskManager.h"
#include "../systems/RenderSystem.h"
#include <iostream>
#include <stdexcept>

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

    std::cout << "Game engine initialized" << std::endl;
}

GameEngine::~GameEngine() {
    // Clean up systems
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

    // TODO: Add other system updates here:
    // - Collision detection
    // - Physics
    // - AI
    // - Audio
}

void GameEngine::render() {
    // Clear screen (black background like original arcade)
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
    SDL_RenderClear(renderer_);

    // TODO: Render game objects via render system
    renderSystem_->render();

    // Temporary: Draw a test pattern to verify rendering works
    // Draw a white border around the screen
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    SDL_Rect border = {0, 0, logicalWidth_, logicalHeight_};
    SDL_RenderDrawRect(renderer_, &border);

    // Draw test text "SINISTAR" in the center (using simple rectangles)
    SDL_Rect testRect = {
        logicalWidth_ / 2 - 32,
        logicalHeight_ / 2 - 8,
        64,
        16
    };
    SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer_, &testRect);

    // Present frame
    SDL_RenderPresent(renderer_);
}
