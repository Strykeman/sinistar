/**
 * GameEngine.h
 *
 * Main game engine class - handles initialization, game loop,
 * and coordination of all game systems.
 *
 * Replicates the executive loop from the original EXEC.SRC
 */

#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <SDL2/SDL.h>
#include <memory>
#include <string>

class TaskManager;
class RenderSystem;
class PhysicsSystem;
class TextRenderer;
class Player;

class GameEngine {
public:
    /**
     * Constructor
     * @param windowWidth Width of the window in pixels
     * @param windowHeight Height of the window in pixels
     * @param logicalWidth Logical game width (original arcade resolution)
     * @param logicalHeight Logical game height (original arcade resolution)
     */
    GameEngine(int windowWidth, int windowHeight,
               int logicalWidth, int logicalHeight);

    /**
     * Destructor
     */
    ~GameEngine();

    /**
     * Main game loop - runs until quit
     * Replicates the executive loop timing and task execution
     */
    void run();

    /**
     * Request engine shutdown
     */
    void quit();

    /**
     * Get SDL renderer
     */
    SDL_Renderer* getRenderer() const { return renderer_; }

    /**
     * Get logical width
     */
    int getLogicalWidth() const { return logicalWidth_; }

    /**
     * Get logical height
     */
    int getLogicalHeight() const { return logicalHeight_; }

private:
    /**
     * Initialize Phase 3 player test
     */
    void initPhase3Player();

    /**
     * Process SDL events (input, window events)
     */
    void handleEvents();

    /**
     * Update game state
     * @param deltaTime Time since last update in seconds
     */
    void update(float deltaTime);

    /**
     * Render current frame
     */
    void render();

    /**
     * Draw HUD (lives, bombs, shield)
     */
    void drawHUD();

    // SDL resources
    SDL_Window* window_;
    SDL_Renderer* renderer_;

    // Game systems
    std::unique_ptr<TaskManager> taskManager_;
    std::unique_ptr<RenderSystem> renderSystem_;
    std::unique_ptr<PhysicsSystem> physicsSystem_;
    std::unique_ptr<TextRenderer> textRenderer_;

    // Game entities
    std::unique_ptr<Player> player_;

    // Game loop control
    bool running_;
    int logicalWidth_;
    int logicalHeight_;

    // Timing (target 60 FPS like original arcade)
    static constexpr float TARGET_FPS = 60.0f;
    static constexpr float FRAME_TIME = 1.0f / TARGET_FPS;
    Uint64 lastFrameTime_;
};

#endif // GAMEENGINE_H
