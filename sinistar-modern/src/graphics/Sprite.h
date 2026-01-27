/**
 * Sprite.h
 *
 * Sprite wrapper class for rendering textures with rotation, scaling, and animation
 */

#ifndef SPRITE_H
#define SPRITE_H

#include <SDL.h>
#include <string>

/**
 * Sprite - Wrapper for SDL_Texture with rendering helpers
 *
 * Features:
 * - Render sprites at position with rotation and scale
 * - Support for sprite sheets (multiple frames in one texture)
 * - Animation frame selection
 * - Centered rendering
 * - Alpha blending support
 */
class Sprite {
public:
    // Construct empty sprite
    Sprite();

    // Construct from texture (single frame)
    Sprite(SDL_Texture* texture, int width, int height);

    // Construct from texture with frames (sprite sheet)
    Sprite(SDL_Texture* texture, int frameWidth, int frameHeight, int frameCount);

    // Destructor (does NOT destroy texture - managed by SpriteManager)
    ~Sprite();

    // Set texture
    void setTexture(SDL_Texture* texture, int width, int height);
    void setTexture(SDL_Texture* texture, int frameWidth, int frameHeight, int frameCount);

    // Render sprite at position (centered)
    void render(SDL_Renderer* renderer, float x, float y,
                float rotation = 0.0f, float scale = 1.0f);

    // Render specific frame from sprite sheet
    void renderFrame(SDL_Renderer* renderer, int frame, float x, float y,
                     float rotation = 0.0f, float scale = 1.0f);

    // Render with color modulation
    void renderTinted(SDL_Renderer* renderer, float x, float y,
                      Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255,
                      float rotation = 0.0f, float scale = 1.0f);

    // Render frame with color modulation
    void renderFrameTinted(SDL_Renderer* renderer, int frame,
                           float x, float y,
                           Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255,
                           float rotation = 0.0f, float scale = 1.0f);

    // Get dimensions
    int getWidth() const { return frameWidth_; }
    int getHeight() const { return frameHeight_; }
    int getFrameCount() const { return frameCount_; }

    // Check if valid
    bool isValid() const { return texture_ != nullptr; }

private:
    // Helper: Render with all parameters
    void renderInternal(SDL_Renderer* renderer, int frame,
                        float x, float y,
                        Uint8 r, Uint8 g, Uint8 b, Uint8 a,
                        float rotation, float scale);

    SDL_Texture* texture_;      // Texture reference (not owned)
    int frameWidth_;            // Width of single frame
    int frameHeight_;           // Height of single frame
    int frameCount_;            // Number of frames in sprite sheet
};

#endif // SPRITE_H
