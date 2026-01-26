/**
 * Timer.cpp
 *
 * Simple timer utility (placeholder for now)
 */

#include <SDL.h>

// Placeholder - will be expanded later for game timing utilities
namespace Timer {
    float getTime() {
        return static_cast<float>(SDL_GetTicks()) / 1000.0f;
    }
}
