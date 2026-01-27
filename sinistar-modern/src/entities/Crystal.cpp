/**
 * Crystal.cpp
 *
 * Implementation of crystal planetoid
 */

#include "Crystal.h"
#include "../core/Math.h"
#include "../graphics/SpriteManager.h"
#include "../graphics/Sprite.h"
#include <cmath>
#include <cstdlib>

Crystal::Crystal()
    : GameObject()
    , health_(3.0f)
    , maxHealth_(3.0f)
    , crystalAmount_(1.0f)
    , radius_(12.0f)
    , rotation_(0.0f)
    , rotationSpeed_(0.5f)
    , pulseTimer_(0.0f)
{
    // Random size variant
    int sizeRoll = rand() % 100;
    if (sizeRoll < 60) {
        size_ = Size::SMALL;
        crystalAmount_ = 1.0f;
        health_ = 2.0f;
        radius_ = 10.0f;
    } else if (sizeRoll < 90) {
        size_ = Size::MEDIUM;
        crystalAmount_ = 3.0f;
        health_ = 4.0f;
        radius_ = 14.0f;
    } else {
        size_ = Size::LARGE;
        crystalAmount_ = 5.0f;
        health_ = 6.0f;
        radius_ = 18.0f;
    }
    maxHealth_ = health_;

    // Random rotation speed
    rotationSpeed_ = 0.3f + (rand() % 100) / 100.0f * 0.5f;
    if (rand() % 2 == 0) {
        rotationSpeed_ *= -1.0f;
    }

    setActive(true);
}

Crystal::~Crystal() {
}

void Crystal::update(float deltaTime) {
    if (!isActive()) return;

    // Rotate
    rotation_ += rotationSpeed_ * deltaTime;
    if (rotation_ > 2.0f * M_PI) {
        rotation_ -= 2.0f * M_PI;
    }

    // Pulse effect
    pulseTimer_ += deltaTime * 2.0f;
    if (pulseTimer_ > 2.0f * M_PI) {
        pulseTimer_ -= 2.0f * M_PI;
    }

    // Deactivate if depleted
    if (isDepleted()) {
        setActive(false);
    }

    GameObject::update(deltaTime);
}

void Crystal::render(SDL_Renderer* renderer) {
    if (!isActive()) return;

    Vector2 pos = getPosition();

    // Calculate pulse intensity (0.8 to 1.0)
    float pulse = 0.8f + 0.2f * (1.0f + std::sin(pulseTimer_)) / 2.0f;

    // Health-based color (green when full, yellow when damaged, red when low)
    float healthRatio = health_ / maxHealth_;
    SDL_Color color;
    if (healthRatio > 0.66f) {
        // Green to yellow-green
        color = {
            static_cast<Uint8>(100 + (155 * (1.0f - healthRatio) * 3.0f)),
            static_cast<Uint8>(255 * pulse),
            static_cast<Uint8>(100 * pulse),
            255
        };
    } else if (healthRatio > 0.33f) {
        // Yellow-green to yellow
        color = {
            static_cast<Uint8>(255 * pulse),
            static_cast<Uint8>(255 * pulse),
            static_cast<Uint8>(50 * pulse),
            255
        };
    } else {
        // Yellow to red
        color = {
            static_cast<Uint8>(255 * pulse),
            static_cast<Uint8>(150 * healthRatio * 3.0f * pulse),
            0,
            255
        };
    }

    // Determine sprite filename based on size
    const char* spriteFilename = nullptr;
    switch (size_) {
        case Size::SMALL:
            spriteFilename = "crystal_small.png";
            break;
        case Size::MEDIUM:
            spriteFilename = "crystal_medium.png";
            break;
        case Size::LARGE:
            spriteFilename = "crystal_large.png";
            break;
    }

    // Try sprite rendering first
    SDL_Texture* crystalSprite = SpriteManager::getInstance().getSprite(spriteFilename);

    if (crystalSprite) {
        // SPRITE-BASED RENDERING
        int spriteWidth, spriteHeight;
        if (SpriteManager::getInstance().getSpriteDimensions(spriteFilename,
                                                              spriteWidth, spriteHeight)) {
            // Map rotation angle to frame (6 rotation frames)
            // Normalize rotation to 0-2π range
            float normalizedAngle = rotation_;
            while (normalizedAngle < 0) normalizedAngle += 2.0f * M_PI;
            while (normalizedAngle >= 2.0f * M_PI) normalizedAngle -= 2.0f * M_PI;

            int frame = static_cast<int>((normalizedAngle / (2.0f * M_PI)) * 6.0f) % 6;

            // Create sprite wrapper (6 frames horizontal)
            // Small: 96x16 (16x16 per frame)
            // Medium: 144x24 (24x24 per frame)
            // Large: 192x32 (32x32 per frame)
            Sprite sprite(crystalSprite, spriteWidth / 6, spriteHeight, 6);

            // Render with health-based color tinting and pulse effect
            sprite.renderFrameTinted(renderer, frame, pos.x, pos.y,
                                    color.r, color.g, color.b, color.a,
                                    0.0f, 1.0f);  // No rotation needed - frame already rotated
        }
    } else {
        // GEOMETRIC FALLBACK
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

        // Draw octagon (8-sided crystal)
        const int sides = 8;
        SDL_Point points[sides + 1];

        for (int i = 0; i <= sides; i++) {
            float angle = rotation_ + (2.0f * M_PI * i) / sides;
            float r = radius_ * (0.9f + 0.1f * std::sin(pulseTimer_ + i));
            points[i].x = static_cast<int>(pos.x + r * std::cos(angle));
            points[i].y = static_cast<int>(pos.y + r * std::sin(angle));
        }

        SDL_RenderDrawLines(renderer, points, sides + 1);

        // Draw inner crystal segments based on size
        int segments = static_cast<int>(size_) + 1;
        for (int s = 0; s < segments; s++) {
            float innerRadius = radius_ * 0.3f + (s * radius_ * 0.2f);
            SDL_Point innerPoints[sides + 1];

            for (int i = 0; i <= sides; i++) {
                float angle = -rotation_ * 0.5f + (2.0f * M_PI * i) / sides + s * 0.5f;
                innerPoints[i].x = static_cast<int>(pos.x + innerRadius * std::cos(angle));
                innerPoints[i].y = static_cast<int>(pos.y + innerRadius * std::sin(angle));
            }

            SDL_RenderDrawLines(renderer, innerPoints, sides + 1);
        }

        // Draw center dot (brighter)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect centerDot = {
            static_cast<int>(pos.x - 1),
            static_cast<int>(pos.y - 1),
            3, 3
        };
        SDL_RenderFillRect(renderer, &centerDot);
    }
}

bool Crystal::takeDamage(float amount) {
    if (!isActive()) return false;

    health_ -= amount;

    if (health_ <= 0.0f) {
        health_ = 0.0f;
        setActive(false);
        return true;  // Destroyed
    }

    return false;  // Damaged but not destroyed
}

float Crystal::mine(float amount) {
    if (!isActive() || isDepleted()) return 0.0f;

    float mined = std::min(amount, crystalAmount_);
    crystalAmount_ -= mined;

    // Mining also damages the crystal
    takeDamage(amount * 0.5f);

    return mined;
}
