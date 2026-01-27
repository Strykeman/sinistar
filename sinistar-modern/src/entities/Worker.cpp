/**
 * Worker.cpp
 *
 * Implementation of Worker enemy AI
 */

#include "Worker.h"
#include "../core/Math.h"
#include "../systems/PhysicsSystem.h"
#include "../graphics/SpriteManager.h"
#include "../graphics/Sprite.h"
#include <cmath>
#include <cstdlib>

Worker::Worker()
    : GameObject()
    , state_(WorkerState::SEARCHING)
    , targetCrystal_(nullptr)
    , player_(nullptr)
    , health_(1.0f)
    , maxHealth_(1.0f)
    , thrustPower_(60.0f)      // Slower than Warrior
    , maxSpeed_(100.0f)        // Slower than Warrior
    , radius_(6.0f)
    , miningTimer_(0.0f)
    , miningRate_(0.3f)        // Mine 0.3 crystals per second
    , carryingCrystal_(0.0f)
    , maxCrystalCarry_(1.0f)
    , stateTimer_(0.0f)
    , returnTarget_(128.0f, 122.0f)  // Center of screen (will be Sinistar position)
    , rotation_(0.0f)
    , pulseTimer_(0.0f)
{
    setActive(true);
}

Worker::~Worker() {
}

void Worker::update(float deltaTime) {
    if (!isActive()) return;

    // Check for player threat
    if (isPlayerThreat() && state_ != WorkerState::FLEEING) {
        state_ = WorkerState::FLEEING;
        stateTimer_ = 2.0f;  // Flee for 2 seconds
    }

    // Execute current state
    executeState(deltaTime);

    // Update state timer
    stateTimer_ -= deltaTime;

    // Update visual effects
    pulseTimer_ += deltaTime * 3.0f;
    if (pulseTimer_ > 2.0f * M_PI) {
        pulseTimer_ -= 2.0f * M_PI;
    }

    // Update rotation based on velocity
    if (getVelocity().length() > 0.1f) {
        rotation_ = getVelocity().angle();
    }

    GameObject::update(deltaTime);
}

void Worker::executeState(float deltaTime) {
    switch (state_) {
        case WorkerState::SEARCHING:
            stateSearching(deltaTime);
            break;

        case WorkerState::MINING:
            stateMining(deltaTime);
            break;

        case WorkerState::RETURNING:
            stateReturning(deltaTime);
            break;

        case WorkerState::FLEEING:
            stateFleeing(deltaTime);
            break;
    }
}

void Worker::stateSearching(float deltaTime) {
    // If we have a target crystal, move toward it
    if (targetCrystal_ && targetCrystal_->isActive() && !targetCrystal_->isDepleted()) {
        Vector2 toTarget = targetCrystal_->getPosition() - getPosition();
        float distance = toTarget.length();

        // If close enough, start mining
        if (distance < radius_ + targetCrystal_->getRadius() + 5.0f) {
            state_ = WorkerState::MINING;
            setVelocity(0.0f, 0.0f);  // Stop moving
            return;
        }

        // Move toward crystal
        Vector2 direction = toTarget.normalized();
        Vector2 acceleration = direction * thrustPower_;

        Vector2 newVelocity = PhysicsSystem::applyAcceleration(
            getVelocity(), acceleration, maxSpeed_, deltaTime
        );
        setVelocity(newVelocity);
    } else {
        // No target or target destroyed - wander
        targetCrystal_ = nullptr;

        // Apply drag to slow down
        Vector2 newVelocity = getVelocity() * 0.95f;
        setVelocity(newVelocity);

        // Occasionally thrust in random direction
        if (stateTimer_ <= 0.0f) {
            float randomAngle = (rand() % 360) * M_PI / 180.0f;
            Vector2 randomDir = Vector2::fromAngle(randomAngle);
            Vector2 acceleration = randomDir * thrustPower_ * 0.5f;

            newVelocity = PhysicsSystem::applyAcceleration(
                getVelocity(), acceleration, maxSpeed_ * 0.5f, deltaTime
            );
            setVelocity(newVelocity);

            stateTimer_ = 1.0f + (rand() % 200) / 100.0f;  // 1-3 seconds
        }
    }
}

void Worker::stateMining(float deltaTime) {
    // Check if crystal still exists
    if (!targetCrystal_ || !targetCrystal_->isActive() || targetCrystal_->isDepleted()) {
        state_ = WorkerState::SEARCHING;
        targetCrystal_ = nullptr;
        return;
    }

    // Check if full
    if (carryingCrystal_ >= maxCrystalCarry_) {
        state_ = WorkerState::RETURNING;
        return;
    }

    // Mine crystal
    miningTimer_ += deltaTime;
    if (miningTimer_ >= 1.0f / miningRate_) {
        float mined = targetCrystal_->mine(miningRate_ * deltaTime);
        carryingCrystal_ += mined;
        miningTimer_ = 0.0f;

        // If crystal depleted or we're full, stop mining
        if (targetCrystal_->isDepleted() || carryingCrystal_ >= maxCrystalCarry_) {
            state_ = WorkerState::RETURNING;
        }
    }

    // Stay near crystal while mining
    Vector2 toTarget = targetCrystal_->getPosition() - getPosition();
    if (toTarget.length() > radius_ + targetCrystal_->getRadius() + 8.0f) {
        // Drifted too far, move back
        Vector2 direction = toTarget.normalized();
        Vector2 newVelocity = direction * 20.0f;
        setVelocity(newVelocity);
    } else {
        // Stay still
        setVelocity(getVelocity() * 0.9f);
    }
}

void Worker::stateReturning(float deltaTime) {
    // Move toward return point (Sinistar location)
    Vector2 toTarget = returnTarget_ - getPosition();
    float distance = toTarget.length();

    // If reached destination, deliver crystal
    if (distance < 30.0f) {
        // Delivered! (In full game, this would contribute to building Sinistar)
        carryingCrystal_ = 0.0f;
        state_ = WorkerState::SEARCHING;
        targetCrystal_ = nullptr;
        return;
    }

    // Move toward return point
    Vector2 direction = toTarget.normalized();
    Vector2 acceleration = direction * thrustPower_ * 1.2f;  // Faster when returning

    Vector2 newVelocity = PhysicsSystem::applyAcceleration(
        getVelocity(), acceleration, maxSpeed_ * 1.2f, deltaTime
    );
    setVelocity(newVelocity);
}

void Worker::stateFleeing(float deltaTime) {
    if (!player_) {
        state_ = carryingCrystal_ > 0.0f ? WorkerState::RETURNING : WorkerState::SEARCHING;
        return;
    }

    // Flee timer expired
    if (stateTimer_ <= 0.0f) {
        state_ = carryingCrystal_ > 0.0f ? WorkerState::RETURNING : WorkerState::SEARCHING;
        return;
    }

    // Flee away from player
    Vector2 fromPlayer = getPosition() - player_->getPosition();
    if (fromPlayer.length() < 0.1f) {
        // Too close, pick random direction
        float randomAngle = (rand() % 360) * M_PI / 180.0f;
        fromPlayer = Vector2::fromAngle(randomAngle);
    }

    Vector2 fleeDirection = fromPlayer.normalized();
    Vector2 acceleration = fleeDirection * thrustPower_ * 1.5f;  // Fast flee

    Vector2 newVelocity = PhysicsSystem::applyAcceleration(
        getVelocity(), acceleration, maxSpeed_ * 1.3f, deltaTime
    );
    setVelocity(newVelocity);
}

bool Worker::isPlayerThreat() const {
    if (!player_ || !player_->isActive()) return false;

    float distance = MathUtils::distance(getPosition(), player_->getPosition());
    return distance < 80.0f;  // Flee if player within 80 units
}

void Worker::render(SDL_Renderer* renderer) {
    if (!isActive()) return;

    Vector2 pos = getPosition();

    // Color based on state
    SDL_Color color;
    switch (state_) {
        case WorkerState::SEARCHING:
            color = {100, 100, 255, 255};  // Blue
            break;
        case WorkerState::MINING:
            color = {255, 255, 100, 255};  // Yellow (mining)
            break;
        case WorkerState::RETURNING:
            color = {100, 255, 100, 255};  // Green (has crystal)
            break;
        case WorkerState::FLEEING:
            color = {255, 150, 100, 255};  // Orange (fleeing)
            break;
    }

    // Pulse effect
    float pulse = 0.8f + 0.2f * (1.0f + std::sin(pulseTimer_)) / 2.0f;
    color.r = static_cast<Uint8>(color.r * pulse);
    color.g = static_cast<Uint8>(color.g * pulse);
    color.b = static_cast<Uint8>(color.b * pulse);

    // Try sprite rendering first
    SDL_Texture* workerSprite = SpriteManager::getInstance().getSprite("worker.png");

    if (workerSprite) {
        // SPRITE-BASED RENDERING
        int spriteWidth, spriteHeight;
        if (SpriteManager::getInstance().getSpriteDimensions("worker.png",
                                                              spriteWidth, spriteHeight)) {
            // Determine which frame to use based on carrying state
            // Frame 0: Empty
            // Frame 1: Carrying crystal
            int frame = (carryingCrystal_ > 0.0f) ? 1 : 0;

            // Create sprite wrapper (2 frames horizontal: 28x14 total, 14x14 per frame)
            Sprite sprite(workerSprite, spriteWidth / 2, spriteHeight, 2);

            // Render with state-based color tinting and pulse effect
            sprite.renderFrameTinted(renderer, frame, pos.x, pos.y,
                                    color.r, color.g, color.b, color.a,
                                    rotation_, 1.0f);
        }
    } else {
        // GEOMETRIC FALLBACK
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

        // Draw hexagon (6-sided worker)
        const int sides = 6;
        SDL_Point points[sides + 1];

        for (int i = 0; i <= sides; i++) {
            float angle = rotation_ + (2.0f * M_PI * i) / sides;
            points[i].x = static_cast<int>(pos.x + radius_ * std::cos(angle));
            points[i].y = static_cast<int>(pos.y + radius_ * std::sin(angle));
        }

        SDL_RenderDrawLines(renderer, points, sides + 1);

        // If carrying crystal, draw indicator
        if (carryingCrystal_ > 0.0f) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_Rect crystalIndicator = {
                static_cast<int>(pos.x - 2),
                static_cast<int>(pos.y - 2),
                4, 4
            };
            SDL_RenderFillRect(renderer, &crystalIndicator);
        }

        // Draw direction indicator
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 200);
        float frontX = pos.x + radius_ * 1.2f * std::cos(rotation_);
        float frontY = pos.y + radius_ * 1.2f * std::sin(rotation_);
        SDL_RenderDrawLine(
            renderer,
            static_cast<int>(pos.x),
            static_cast<int>(pos.y),
            static_cast<int>(frontX),
            static_cast<int>(frontY)
        );
    }
}

bool Worker::takeDamage(float amount) {
    if (!isActive()) return false;

    health_ -= amount;

    if (health_ <= 0.0f) {
        health_ = 0.0f;
        setActive(false);
        return true;  // Destroyed
    }

    return false;  // Damaged but not destroyed
}
