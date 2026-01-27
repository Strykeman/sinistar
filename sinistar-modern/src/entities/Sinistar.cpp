/**
 * Sinistar.cpp
 *
 * Implementation of Sinistar boss
 */

#include "Sinistar.h"
#include "../core/Math.h"
#include "../systems/PhysicsSystem.h"
#include "../graphics/SpriteManager.h"
#include "../graphics/Sprite.h"
#include <cmath>
#include <cstdlib>

Sinistar::Sinistar()
    : GameObject()
    , state_(SinistarState::BUILDING)
    , target_(nullptr)
    , constructionProgress_(0.0f)
    , crystalsNeeded_(20.0f)        // Needs 20 crystals to complete
    , crystalsCollected_(0.0f)
    , health_(20.0f)                // Very tanky boss
    , maxHealth_(20.0f)
    , thrustPower_(150.0f)          // Fast and aggressive
    , maxSpeed_(180.0f)             // Faster than player
    , radius_(30.0f)                // Large boss
    , attackRange_(100.0f)          // Start attacking within 100 units
    , biteRange_(40.0f)             // Bite within 40 units
    , biteDamage_(1.0f)             // Heavy damage
    , biteTimer_(0.0f)
    , biteCooldown_(2.0f)           // Bite every 2 seconds
    , stateTimer_(0.0f)
    , awakeningDuration_(3.0f)      // 3 second wake animation
    , rotation_(0.0f)
    , pulseTimer_(0.0f)
    , jawAngle_(0.0f)
    , scale_(0.1f)                  // Start small during construction
    , currentVoiceLine_("")
    , voiceTimer_(0.0f)
    , voiceCooldown_(5.0f)          // Voice line every 5 seconds
    , deathTimer_(0.0f)
    , deathDuration_(2.0f)
{
    // Position at center of screen initially
    setPosition(128.0f, 122.0f);
    setActive(false);  // Not active until constructed
}

Sinistar::~Sinistar() {
}

void Sinistar::update(float deltaTime) {
    // Execute current state
    executeState(deltaTime);

    // Update visual effects
    pulseTimer_ += deltaTime * 4.0f;
    if (pulseTimer_ > 2.0f * M_PI) {
        pulseTimer_ -= 2.0f * M_PI;
    }

    // Update voice timer
    if (!currentVoiceLine_.empty()) {
        voiceTimer_ += deltaTime;
        if (voiceTimer_ >= 3.0f) {  // Show voice for 3 seconds
            currentVoiceLine_ = "";
            voiceTimer_ = 0.0f;
        }
    }

    GameObject::update(deltaTime);
}

void Sinistar::executeState(float deltaTime) {
    switch (state_) {
        case SinistarState::BUILDING:
            stateBuilding(deltaTime);
            break;

        case SinistarState::AWAKENING:
            stateAwakening(deltaTime);
            break;

        case SinistarState::HUNTING:
            stateHunting(deltaTime);
            break;

        case SinistarState::ATTACKING:
            stateAttacking(deltaTime);
            break;

        case SinistarState::BITING:
            stateBiting(deltaTime);
            break;

        case SinistarState::DYING:
            stateDying(deltaTime);
            break;

        case SinistarState::DEAD:
            // Do nothing
            break;
    }

    stateTimer_ += deltaTime;
}

void Sinistar::stateBuilding(float deltaTime) {
    // Slowly rotate during construction
    rotation_ += deltaTime * 0.3f;

    // Scale grows with construction progress
    scale_ = 0.1f + constructionProgress_ * 0.9f;

    // Not active yet
    setActive(false);
}

void Sinistar::stateAwakening(float deltaTime) {
    // Grow to full size
    scale_ = 0.7f + stateTimer_ / awakeningDuration_ * 0.3f;
    if (scale_ > 1.0f) scale_ = 1.0f;

    // Pulsing rotation
    rotation_ += deltaTime * std::sin(stateTimer_ * 3.0f) * 2.0f;

    // Awakening complete?
    if (stateTimer_ >= awakeningDuration_) {
        state_ = SinistarState::HUNTING;
        stateTimer_ = 0.0f;
        scale_ = 1.0f;
        speak("RUN COWARD!");
    }
}

void Sinistar::stateHunting(float deltaTime) {
    if (!target_ || !target_->isActive()) {
        // No target, just drift
        Vector2 newVelocity = getVelocity() * 0.98f;
        setVelocity(newVelocity);
        return;
    }

    // Chase player aggressively
    Vector2 toTarget = target_->getPosition() - getPosition();
    float distance = toTarget.length();

    // Close enough to attack?
    if (distance < attackRange_) {
        state_ = SinistarState::ATTACKING;
        stateTimer_ = 0.0f;
        speak("I HUNGER!");
        return;
    }

    // Hunt!
    Vector2 direction = toTarget.normalized();
    Vector2 acceleration = direction * thrustPower_;

    Vector2 newVelocity = PhysicsSystem::applyAcceleration(
        getVelocity(), acceleration, maxSpeed_, deltaTime
    );
    setVelocity(newVelocity);

    // Face target
    rotation_ = toTarget.angle();

    // Occasional taunts
    voiceCooldown_ -= deltaTime;
    if (voiceCooldown_ <= 0.0f && currentVoiceLine_.empty()) {
        int taunt = rand() % 3;
        switch (taunt) {
            case 0: speak("RUN! RUN! RUN!"); break;
            case 1: speak("FLEE BEFORE ME!"); break;
            case 2: speak("YOU CANNOT ESCAPE!"); break;
        }
        voiceCooldown_ = 8.0f;
    }
}

void Sinistar::stateAttacking(float deltaTime) {
    if (!target_ || !target_->isActive()) {
        state_ = SinistarState::HUNTING;
        stateTimer_ = 0.0f;
        return;
    }

    Vector2 toTarget = target_->getPosition() - getPosition();
    float distance = toTarget.length();

    // Too far? Go back to hunting
    if (distance > attackRange_ * 1.5f) {
        state_ = SinistarState::HUNTING;
        stateTimer_ = 0.0f;
        return;
    }

    // Close enough to bite?
    if (distance < biteRange_) {
        state_ = SinistarState::BITING;
        stateTimer_ = 0.0f;
        biteTimer_ = 0.0f;
        speak("I AM SINISTAR!");
        return;
    }

    // Aggressively approach for bite
    Vector2 direction = toTarget.normalized();
    Vector2 acceleration = direction * thrustPower_ * 1.5f;  // Extra aggressive

    Vector2 newVelocity = PhysicsSystem::applyAcceleration(
        getVelocity(), acceleration, maxSpeed_ * 1.3f, deltaTime
    );
    setVelocity(newVelocity);

    // Face target
    rotation_ = toTarget.angle();

    // Jaw opening animation
    jawAngle_ = std::sin(stateTimer_ * 10.0f) * 0.3f;
}

void Sinistar::stateBiting(float deltaTime) {
    biteTimer_ += deltaTime;

    // Bite animation - jaw snaps shut
    if (biteTimer_ < 0.3f) {
        // Opening
        jawAngle_ = (biteTimer_ / 0.3f) * 0.8f;
    } else if (biteTimer_ < 0.5f) {
        // Snapping shut!
        jawAngle_ = 0.8f - ((biteTimer_ - 0.3f) / 0.2f) * 0.8f;
    } else {
        jawAngle_ = 0.0f;
    }

    // Lunge forward during bite
    if (biteTimer_ < 0.5f && target_) {
        Vector2 toTarget = target_->getPosition() - getPosition();
        Vector2 direction = toTarget.normalized();
        Vector2 lungeVelocity = direction * 300.0f;  // Fast lunge
        setVelocity(lungeVelocity);
    } else {
        // Slow down after bite
        setVelocity(getVelocity() * 0.9f);
    }

    // Bite complete?
    if (biteTimer_ >= biteCooldown_) {
        state_ = SinistarState::HUNTING;
        stateTimer_ = 0.0f;
        jawAngle_ = 0.0f;
    }
}

void Sinistar::stateDying(float deltaTime) {
    deathTimer_ += deltaTime;

    // Spin and shrink
    rotation_ += deltaTime * 5.0f;
    scale_ = 1.0f - (deathTimer_ / deathDuration_);
    if (scale_ < 0.0f) scale_ = 0.0f;

    // Slow down
    setVelocity(getVelocity() * 0.95f);

    // Death complete?
    if (deathTimer_ >= deathDuration_) {
        state_ = SinistarState::DEAD;
        setActive(false);
    }
}

bool Sinistar::addCrystal(float amount) {
    if (state_ != SinistarState::BUILDING) return false;

    crystalsCollected_ += amount;
    constructionProgress_ = crystalsCollected_ / crystalsNeeded_;

    if (constructionProgress_ >= 1.0f) {
        constructionProgress_ = 1.0f;
        return true;  // Construction complete!
    }

    return false;
}

float Sinistar::getConstructionProgress() const {
    return constructionProgress_;
}

void Sinistar::activate() {
    if (state_ != SinistarState::BUILDING) return;

    state_ = SinistarState::AWAKENING;
    stateTimer_ = 0.0f;
    setActive(true);
    speak("BEWARE, I LIVE!");
}

bool Sinistar::takeDamage(float amount) {
    if (state_ == SinistarState::BUILDING ||
        state_ == SinistarState::DYING ||
        state_ == SinistarState::DEAD) {
        return false;
    }

    health_ -= amount;

    if (health_ <= 0.0f) {
        health_ = 0.0f;
        state_ = SinistarState::DYING;
        stateTimer_ = 0.0f;
        deathTimer_ = 0.0f;
        speak("NOOOOOO!");
        return true;  // Destroyed
    }

    // Damage reaction
    if (rand() % 3 == 0 && currentVoiceLine_.empty()) {
        int reaction = rand() % 3;
        switch (reaction) {
            case 0: speak("YOU DARE?!"); break;
            case 1: speak("FOOLISH!"); break;
            case 2: speak("I LIVE!"); break;
        }
    }

    return false;
}

void Sinistar::speak(const std::string& line) {
    currentVoiceLine_ = line;
    voiceTimer_ = 0.0f;
}

void Sinistar::render(SDL_Renderer* renderer) {
    if (!isActive() && state_ != SinistarState::BUILDING) return;

    Vector2 pos = getPosition();

    // Apply scale
    float renderRadius = radius_ * scale_;

    // Color based on state
    SDL_Color color;
    switch (state_) {
        case SinistarState::BUILDING:
            color = {100, 100, 100, 255};  // Gray (under construction)
            break;
        case SinistarState::AWAKENING:
            {
                float pulse = 0.5f + 0.5f * std::sin(pulseTimer_);
                color = {
                    static_cast<Uint8>(255 * pulse),
                    static_cast<Uint8>(100 * pulse),
                    0,
                    255
                };
            }
            break;
        case SinistarState::HUNTING:
        case SinistarState::ATTACKING:
        case SinistarState::BITING:
            {
                float pulse = 0.8f + 0.2f * std::sin(pulseTimer_);
                float healthRatio = health_ / maxHealth_;
                color = {
                    static_cast<Uint8>(255 * pulse),
                    static_cast<Uint8>(50 * healthRatio * pulse),
                    0,
                    255
                };
            }
            break;
        case SinistarState::DYING:
            {
                float fade = 1.0f - (deathTimer_ / deathDuration_);
                color = {
                    static_cast<Uint8>(255 * fade),
                    static_cast<Uint8>(100 * fade),
                    static_cast<Uint8>(100 * fade),
                    255
                };
            }
            break;
        case SinistarState::DEAD:
            return;  // Don't render when dead
    }

    // Try to use sprite rendering first
    SDL_Texture* faceSprite = SpriteManager::getInstance().getSprite("sinistar_face.png");

    if (faceSprite) {
        // Sprite-based rendering
        int spriteWidth, spriteHeight;
        if (SpriteManager::getInstance().getSpriteDimensions("sinistar_face.png", spriteWidth, spriteHeight)) {
            // Calculate which frame to show based on jaw animation
            int frame = 0;
            if (state_ == SinistarState::BITING) {
                // Jaw opening animation (frames 0-3)
                frame = static_cast<int>((jawAngle_ / 0.8f) * 3);  // jawAngle_ max is 0.8
                if (frame > 3) frame = 3;
            } else if (state_ == SinistarState::AWAKENING || pulseTimer_ > 0) {
                // Eye glow animation (frames 4-5)
                frame = 4 + (static_cast<int>(pulseTimer_ * 4) % 2);
            }

            // Create sprite wrapper for rendering
            Sprite sprite(faceSprite, spriteWidth / 6, spriteHeight, 6);  // 6 frames horizontal

            // Render with color tinting and scaling
            sprite.renderFrameTinted(
                renderer,
                frame,
                pos.x, pos.y,
                color.r, color.g, color.b, color.a,
                rotation_,
                scale_
            );
        }
    } else {
        // Fallback: Geometric rendering (existing code)
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    // Draw large triangular face (menacing)
    // Top point
    float topX = pos.x + renderRadius * std::cos(rotation_ - M_PI / 2);
    float topY = pos.y + renderRadius * std::sin(rotation_ - M_PI / 2);

    // Bottom left
    float leftX = pos.x + renderRadius * std::cos(rotation_ + 2.0f * M_PI / 3.0f);
    float leftY = pos.y + renderRadius * std::sin(rotation_ + 2.0f * M_PI / 3.0f);

    // Bottom right
    float rightX = pos.x + renderRadius * std::cos(rotation_ - 2.0f * M_PI / 3.0f);
    float rightY = pos.y + renderRadius * std::sin(rotation_ - 2.0f * M_PI / 3.0f);

    // Draw triangle
    SDL_RenderDrawLine(renderer, topX, topY, leftX, leftY);
    SDL_RenderDrawLine(renderer, leftX, leftY, rightX, rightY);
    SDL_RenderDrawLine(renderer, rightX, rightY, topX, topY);

    // Draw inner details (eyes and mouth)
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red eyes/mouth

    // Eyes
    float eyeOffset = renderRadius * 0.3f;
    float eyeY = pos.y - renderRadius * 0.2f;

    SDL_Rect leftEye = {
        static_cast<int>(pos.x - eyeOffset - 3),
        static_cast<int>(eyeY - 3),
        6, 6
    };
    SDL_RenderFillRect(renderer, &leftEye);

    SDL_Rect rightEye = {
        static_cast<int>(pos.x + eyeOffset - 3),
        static_cast<int>(eyeY - 3),
        6, 6
    };
    SDL_RenderFillRect(renderer, &rightEye);

    // Mouth (jaw animation)
    float mouthY = pos.y + renderRadius * 0.2f;
    float jawOffset = renderRadius * 0.4f * jawAngle_;

    SDL_RenderDrawLine(
        renderer,
        pos.x - renderRadius * 0.4f, mouthY,
        pos.x + renderRadius * 0.4f, mouthY
    );

        // Lower jaw (opens during bite)
        SDL_RenderDrawLine(
            renderer,
            pos.x - renderRadius * 0.3f, mouthY + jawOffset,
            pos.x + renderRadius * 0.3f, mouthY + jawOffset
        );
    }  // End of geometric fallback

    // Construction progress indicator (shown regardless of rendering mode)
    if (state_ == SinistarState::BUILDING) {
        SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);
        int barWidth = static_cast<int>(renderRadius * 2 * constructionProgress_);
        SDL_Rect progressBar = {
            static_cast<int>(pos.x - renderRadius),
            static_cast<int>(pos.y + renderRadius + 5),
            barWidth,
            3
        };
        SDL_RenderFillRect(renderer, &progressBar);
    }
}
