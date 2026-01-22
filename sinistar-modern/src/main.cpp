/**
 * Sinistar Modern - Cross-Platform Remake
 *
 * Main entry point for the game
 * Original arcade game (1982) by Williams Electronics
 * Modern recreation using SDL2
 */

#include <SDL2/SDL.h>
#include <iostream>
#include <memory>
#include "core/GameEngine.h"

// Screen dimensions matching original arcade (will be scaled)
constexpr int ORIGINAL_WIDTH = 256;
constexpr int ORIGINAL_HEIGHT = 244;
constexpr int WINDOW_SCALE = 3; // 3x scaling for modern displays

int main(int argc, char* argv[]) {
    (void)argc; // Unused
    (void)argv; // Unused

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    std::cout << "Sinistar Modern v1.0.0" << std::endl;
    std::cout << "SDL " << SDL_MAJOR_VERSION << "." << SDL_MINOR_VERSION
              << "." << SDL_PATCHLEVEL << " initialized" << std::endl;

    try {
        // Create game engine
        auto engine = std::make_unique<GameEngine>(
            ORIGINAL_WIDTH * WINDOW_SCALE,
            ORIGINAL_HEIGHT * WINDOW_SCALE,
            ORIGINAL_WIDTH,
            ORIGINAL_HEIGHT
        );

        // Run game loop
        engine->run();

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Clean shutdown
    SDL_Quit();
    std::cout << "Sinistar terminated normally" << std::endl;

    return 0;
}
