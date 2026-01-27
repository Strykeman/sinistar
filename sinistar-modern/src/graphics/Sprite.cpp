/**
 * Sprite.cpp
 *
 * Implementation of sprite rendering helpers
 */

#include "Sprite.h"
#include <cmath>

Sprite::Sprite()
    : texture_(nullptr)
    , frameWidth_(0)
    , frameHeight_(0)
    , frameCount_(1)
{
}

Sprite::Sprite(SDL_Texture* texture, int width, int height)
    : texture_(texture)
    , frameWidth_(width)
    , frameHeight_(height)
    , frameCount_(1)
{
}

Sprite::Sprite(SDL_Texture* texture, int frameWidth, int frameHeight, int frameCount)
    : texture_(texture)
    , frameWidth_(frameWidth)
    , frameHeight_(frameHeight)
    , frameCount_(frameCount)
{
}

Sprite::~Sprite() {
    // Note: We don't destroy texture - it's managed by SpriteManager
}

void Sprite::setTexture(SDL_Texture* texture, int width, int height) {
    texture_ = texture;
    frameWidth_ = width;
    frameHeight_ = height;
    frameCount_ = 1;
}

void Sprite::setTexture(SDL_Texture* texture, int frameWidth, int frameHeight, int frameCount) {
    texture_ = texture;
    frameWidth_ = frameWidth;
    frameHeight_ = frameHeight;
    frameCount_ = frameCount;
}

void Sprite::render(SDL_Renderer* renderer, float x, float y,
                    float rotation, float scale) {
    renderInternal(renderer, 0, x, y, 255, 255, 255, 255, rotation, scale);
}

void Sprite::renderFrame(SDL_Renderer* renderer, int frame, float x, float y,
                         float rotation, float scale) {
    renderInternal(renderer, frame, x, y, 255, 255, 255, 255, rotation, scale);
}

void Sprite::renderTinted(SDL_Renderer* renderer, float x, float y,
                          Uint8 r, Uint8 g, Uint8 b, Uint8 a,
                          float rotation, float scale) {
    renderInternal(renderer, 0, x, y, r, g, b, a, rotation, scale);
}

void Sprite::renderFrameTinted(SDL_Renderer* renderer, int frame,
                               float x, float y,
                               Uint8 r, Uint8 g, Uint8 b, Uint8 a,
                               float rotation, float scale) {
    renderInternal(renderer, frame, x, y, r, g, b, a, rotation, scale);
}

void Sprite::renderInternal(SDL_Renderer* renderer, int frame,
                            float x, float y,
                            Uint8 r, Uint8 g, Uint8 b, Uint8 a,
                            float rotation, float scale) {
    if (!texture_ || !renderer) {
        return;
    }

    // Clamp frame to valid range
    if (frame < 0) frame = 0;
    if (frame >= frameCount_) frame = frameCount_ - 1;

    // Calculate source rectangle (which frame to render)
    SDL_Rect srcRect;
    srcRect.x = frame * frameWidth_;  // Horizontal sprite sheet
    srcRect.y = 0;
    srcRect.w = frameWidth_;
    srcRect.h = frameHeight_;

    // Calculate destination rectangle (where to render, centered)
    SDL_Rect dstRect;
    dstRect.w = static_cast<int>(frameWidth_ * scale);
    dstRect.h = static_cast<int>(frameHeight_ * scale);
    dstRect.x = static_cast<int>(x - dstRect.w / 2.0f);
    dstRect.y = static_cast<int>(y - dstRect.h / 2.0f);

    // Set color modulation and alpha
    SDL_SetTextureColorMod(texture_, r, g, b);
    SDL_SetTextureAlphaMod(texture_, a);
    SDL_SetTextureBlendMode(texture_, SDL_BLENDMODE_BLEND);

    // Render with rotation
    // Convert rotation from radians to degrees (SDL uses degrees)
    double rotationDegrees = rotation * 180.0 / M_PI;

    SDL_RenderCopyEx(renderer, texture_,
                     &srcRect, &dstRect,
                     rotationDegrees,
                     nullptr,  // Center point (NULL = center of dstRect)
                     SDL_FLIP_NONE);

    // Reset color modulation for next render
    SDL_SetTextureColorMod(texture_, 255, 255, 255);
    SDL_SetTextureAlphaMod(texture_, 255);
}
