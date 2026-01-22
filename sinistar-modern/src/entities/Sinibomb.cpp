/**
 * Sinibomb.cpp
 *
 * Implementation of player projectile
 */

#include "Sinibomb.h"
#include "../graphics/Color.h"
#include <cmath>

Sinibomb::Sinibomb(const Vector2& position, const Vector2& velocity, GameObject* owner)
    : GameObject()
    , owner_(owner)
    , lifetime_(2.0f)         // 2 seconds before auto-explode
    , maxLifetime_(2.0f)
    , damage_(1.0f)           // Standard damage
    , radius_(4.0f)           // 4 pixel collision radius
    , pulseTimer_(0.0f)
{
    setPosition(position);
    setVelocity(velocity);
    setCharacter(1);  // Sinibomb character ID

    // Yellow/orange color for player bombs
    color_ = {255, 200, 0, 255};
}

Sinibomb::~Sinibomb() {
}

void Sinibomb::update(float deltaTime) {
    // Base update (position integration)
    GameObject::update(deltaTime);

    // Update lifetime
    lifetime_ -= deltaTime;

    // Update pulse timer for visual effect
    pulseTimer_ += deltaTime;

    // Auto-explode when lifetime expires
    if (lifetime_ <= 0.0f) {
        explode();
    }
}

void Sinibomb::render(SDL_Renderer* renderer) {
    Vector2 pos = getPosition();

    // Pulsing size effect
    float pulseScale = 1.0f + 0.2f * std::sin(pulseTimer_ * 15.0f);
    float size = radius_ * pulseScale;

    // Draw as a glowing circle (octagon approximation)
    int segments = 8;

    // Outer glow
    SDL_Color glowColor = {color_.r, color_.g, color_.b, 128};
    SDL_SetRenderDrawColor(renderer, glowColor.r, glowColor.g, glowColor.b, glowColor.a);

    for (int i = 0; i < segments; i++) {
        float angle1 = (Math::TWO_PI / segments) * i;
        float angle2 = (Math::TWO_PI / segments) * ((i + 1) % segments);

        Vector2 p1 = pos + Vector2::fromAngle(angle1, size * 1.5f);
        Vector2 p2 = pos + Vector2::fromAngle(angle2, size * 1.5f);

        SDL_RenderDrawLine(renderer,
            static_cast<int>(p1.x), static_cast<int>(p1.y),
            static_cast<int>(p2.x), static_cast<int>(p2.y));
    }

    // Core
    SDL_SetRenderDrawColor(renderer, color_.r, color_.g, color_.b, color_.a);

    for (int i = 0; i < segments; i++) {
        float angle1 = (Math::TWO_PI / segments) * i;
        float angle2 = (Math::TWO_PI / segments) * ((i + 1) % segments);

        Vector2 p1 = pos + Vector2::fromAngle(angle1, size);
        Vector2 p2 = pos + Vector2::fromAngle(angle2, size);

        SDL_RenderDrawLine(renderer,
            static_cast<int>(p1.x), static_cast<int>(p1.y),
            static_cast<int>(p2.x), static_cast<int>(p2.y));
    }

    // Central dot
    SDL_Rect centerDot = {
        static_cast<int>(pos.x - 1),
        static_cast<int>(pos.y - 1),
        2, 2
    };
    SDL_RenderFillRect(renderer, &centerDot);
}

void Sinibomb::explode() {
    setActive(false);
    // TODO: Create explosion effect
    // TODO: Play explosion sound
}
