/**
 * Warrior.cpp
 *
 * Implementation of enemy warrior AI
 */

#include "Warrior.h"
#include "../systems/PhysicsSystem.h"
#include "../graphics/Color.h"
#include "../graphics/SpriteManager.h"
#include "../graphics/Sprite.h"
#include <cmath>
#include <cstdlib>

Warrior::Warrior(WarriorMission mission)
    : GameObject()
    , target_(nullptr)
    , mission_(mission)
    , aiThinkTimer_(0.0f)
    , aiThinkInterval_(0.2f)   // Think 5 times per second
    , thrustPower_(80.0f)      // Slower than player
    , maxSpeed_(120.0f)        // Slower than player
    , turnSpeed_(2.0f)         // Rotation speed
    , drag_(0.95f)             // More drag than player
    , health_(1.0f)            // 1 hit to destroy
    , maxHealth_(1.0f)
    , radius_(6.0f)            // 6 pixel collision radius
    , size_(6.0f)              // 6 pixel visual size
    , rotation_(0.0f)
    , driftTarget_(0, 0)
    , driftTimer_(0.0f)
{
    setCharacter(2);  // Warrior character ID

    // Red color for enemies
    color_ = {255, 0, 0, 255};
}

Warrior::~Warrior() {
}

void Warrior::update(float deltaTime) {
    // Base update
    GameObject::update(deltaTime);

    // Update AI think timer
    aiThinkTimer_ += deltaTime;

    // Execute AI mission
    if (aiThinkTimer_ >= aiThinkInterval_) {
        aiThinkTimer_ = 0.0f;
        executeMission(deltaTime);
    }

    // Apply drag
    Vector2 vel = getVelocity();
    vel = PhysicsSystem::applyDrag(vel, 1.0f - drag_, deltaTime);
    setVelocity(vel);

    // Clamp to max speed
    if (vel.lengthSquared() > maxSpeed_ * maxSpeed_) {
        vel = vel.normalized() * maxSpeed_;
        setVelocity(vel);
    }

    // Update visual rotation to face movement direction
    if (vel.lengthSquared() > 1.0f) {
        float targetRotation = vel.angle();
        rotation_ = targetRotation;
    }
}

void Warrior::render(SDL_Renderer* renderer) {
    Vector2 pos = getPosition();

    // Try sprite rendering first
    SDL_Texture* warriorSprite = SpriteManager::getInstance().getSprite("warrior.png");

    if (warriorSprite) {
        // SPRITE-BASED RENDERING
        int spriteWidth, spriteHeight;
        if (SpriteManager::getInstance().getSpriteDimensions("warrior.png",
                                                              spriteWidth, spriteHeight)) {
            // Map rotation angle to frame (3 rotation frames)
            // Divide 360 degrees into 3 equal sectors (120 degrees each)
            float normalizedAngle = rotation_;
            while (normalizedAngle < 0) normalizedAngle += 2.0f * M_PI;
            while (normalizedAngle >= 2.0f * M_PI) normalizedAngle -= 2.0f * M_PI;

            int frame = static_cast<int>((normalizedAngle / (2.0f * M_PI)) * 3.0f) % 3;

            // Create sprite wrapper (3 frames horizontal: 36x12 total, 12x12 per frame)
            Sprite sprite(warriorSprite, spriteWidth / 3, spriteHeight, 3);

            // Render with red/orange color tinting
            sprite.renderFrameTinted(renderer, frame, pos.x, pos.y,
                                    color_.r, color_.g, color_.b, color_.a,
                                    0.0f, 1.0f);  // No rotation needed - frame already rotated
        }
    } else {
        // GEOMETRIC FALLBACK
        // Draw as a diamond shape (rotated square)
        Vector2 forward = Vector2::fromAngle(rotation_);
        Vector2 right = forward.perpendicular();

        // Diamond vertices
        Vector2 nose = pos + forward * size_;
        Vector2 left = pos + right * size_;
        Vector2 tail = pos - forward * size_;
        Vector2 right_wing = pos - right * size_;

        // Draw filled diamond
        SDL_SetRenderDrawColor(renderer, color_.r, color_.g, color_.b, color_.a);

        SDL_RenderDrawLine(renderer,
            static_cast<int>(nose.x), static_cast<int>(nose.y),
            static_cast<int>(left.x), static_cast<int>(left.y));
        SDL_RenderDrawLine(renderer,
            static_cast<int>(left.x), static_cast<int>(left.y),
            static_cast<int>(tail.x), static_cast<int>(tail.y));
        SDL_RenderDrawLine(renderer,
            static_cast<int>(tail.x), static_cast<int>(tail.y),
            static_cast<int>(right_wing.x), static_cast<int>(right_wing.y));
        SDL_RenderDrawLine(renderer,
            static_cast<int>(right_wing.x), static_cast<int>(right_wing.y),
            static_cast<int>(nose.x), static_cast<int>(nose.y));

        // Draw cross inside for detail
        SDL_RenderDrawLine(renderer,
            static_cast<int>(nose.x), static_cast<int>(nose.y),
            static_cast<int>(tail.x), static_cast<int>(tail.y));
        SDL_RenderDrawLine(renderer,
            static_cast<int>(left.x), static_cast<int>(left.y),
            static_cast<int>(right_wing.x), static_cast<int>(right_wing.y));
    }

    // Health bar if damaged (shown regardless of rendering mode)
    if (health_ < maxHealth_) {
        float healthPercent = health_ / maxHealth_;
        int barWidth = static_cast<int>(size_ * 2.0f);
        int barHeight = 2;
        int barX = static_cast<int>(pos.x - size_);
        int barY = static_cast<int>(pos.y - size_ - 4);

        // Background (red)
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect bgBar = {barX, barY, barWidth, barHeight};
        SDL_RenderFillRect(renderer, &bgBar);

        // Foreground (green)
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_Rect fgBar = {barX, barY, static_cast<int>(barWidth * healthPercent), barHeight};
        SDL_RenderFillRect(renderer, &fgBar);
    }
}

bool Warrior::takeDamage(float amount) {
    health_ -= amount;

    if (health_ <= 0.0f) {
        health_ = 0.0f;
        setActive(false);
        // TODO: Drop crystal/power-up
        // TODO: Play explosion sound
        // TODO: Add to score
        return true;  // Destroyed
    }

    return false;  // Still alive
}

void Warrior::executeMission(float deltaTime) {
    switch (mission_) {
        case WarriorMission::ATTACK:
            missionAttack(deltaTime);
            break;

        case WarriorMission::CHASE:
            missionChase(deltaTime);
            break;

        case WarriorMission::INTERCEPT:
            missionIntercept(deltaTime);
            break;

        case WarriorMission::DRIFT:
            missionDrift(deltaTime);
            break;

        default:
            missionDrift(deltaTime);
            break;
    }
}

void Warrior::missionAttack(float deltaTime) {
    if (!target_) return;

    // Simple direct attack - move toward target
    Vector2 toTarget = target_->getPosition() - getPosition();
    float distance = toTarget.length();

    if (distance > 1.0f) {
        Vector2 direction = toTarget.normalized();
        applyThrust(direction, deltaTime);
    }
}

void Warrior::missionChase(float deltaTime) {
    if (!target_) return;

    // Chase with some evasive maneuvering
    Vector2 toTarget = target_->getPosition() - getPosition();
    float distance = toTarget.length();

    if (distance > 1.0f) {
        Vector2 direction = toTarget.normalized();

        // Add some perpendicular component for evasion
        Vector2 perpendicular = direction.perpendicular();
        float evasion = std::sin(aiThinkTimer_ * 3.0f) * 0.3f;

        Vector2 chaseDirection = (direction + perpendicular * evasion).normalized();
        applyThrust(chaseDirection, deltaTime);
    }
}

void Warrior::missionIntercept(float deltaTime) {
    if (!target_) return;

    // Predict target's future position and intercept
    Vector2 targetPos = target_->getPosition();
    Vector2 targetVel = target_->getVelocity();

    // Predict where target will be in 1 second
    float predictionTime = 1.0f;
    Vector2 predictedPos = targetPos + targetVel * predictionTime;

    // Move toward predicted position
    Vector2 toIntercept = predictedPos - getPosition();
    float distance = toIntercept.length();

    if (distance > 1.0f) {
        Vector2 direction = toIntercept.normalized();
        applyThrust(direction, deltaTime);
    }
}

void Warrior::missionDrift(float deltaTime) {
    // Random drifting movement
    driftTimer_ += deltaTime;

    // Pick new drift target every few seconds
    if (driftTimer_ >= 3.0f) {
        // Random position on screen (assuming 256x244 screen)
        driftTarget_.x = static_cast<float>(rand() % 256);
        driftTarget_.y = static_cast<float>(rand() % 244);
        driftTimer_ = 0.0f;
    }

    // Move toward drift target
    Vector2 toDrift = driftTarget_ - getPosition();
    float distance = toDrift.length();

    if (distance > 10.0f) {
        Vector2 direction = toDrift.normalized();
        applyThrust(direction * 0.5f, deltaTime);  // Slower drift
    }
}

void Warrior::applyThrust(const Vector2& direction, float deltaTime) {
    Vector2 thrust = direction * thrustPower_ * deltaTime;
    Vector2 vel = getVelocity();
    vel += thrust;
    setVelocity(vel);
}
