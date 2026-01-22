/**
 * Player.cpp
 *
 * Implementation of player ship
 */

#include "Player.h"
#include "../input/InputSystem.h"
#include "../systems/PhysicsSystem.h"
#include "../graphics/Color.h"
#include <cmath>

Player::Player()
    : GameObject()
    , rotation_(0.0f)
    , rotationSpeed_(3.0f)        // 3 radians per second
    , thrustPower_(150.0f)        // Thrust acceleration
    , maxSpeed_(200.0f)           // Max velocity
    , drag_(0.98f)                // Drag per second
    , thrusting_(false)
    , thrustTimer_(0.0f)
    , shieldActive_(true)
    , shieldStrength_(1.0f)
    , shieldRechargeRate_(0.2f)   // 20% per second
    , shieldRechargeDelay_(3.0f)  // 3 seconds before recharge
    , timeSinceHit_(0.0f)
    , bombCount_(0)               // Start with no bombs (collect them)
    , fireRate_(3.0f)             // 3 shots per second
    , timeSinceLastShot_(0.0f)
    , lives_(3)                   // Start with 3 lives
    , shipSize_(8.0f)             // 8 pixels
{
    // Set ship colors
    shipColor_ = {0, 255, 255, 255};      // Cyan (player color from palette)
    thrustColor_ = {255, 128, 0, 255};    // Orange
    shieldColor_ = {0, 255, 0, 128};      // Green translucent

    // Set character type
    setCharacter(0); // Player character ID
}

Player::~Player() {
}

void Player::update(float deltaTime) {
    // Base update (position integration happens in physics system)
    GameObject::update(deltaTime);

    // Update timers
    thrustTimer_ += deltaTime;
    timeSinceHit_ += deltaTime;

    // Update shield
    updateShield(deltaTime);

    // Update weapons
    updateWeapons(deltaTime);

    // Apply drag to velocity
    Vector2 vel = getVelocity();
    vel = PhysicsSystem::applyDrag(vel, 1.0f - drag_, deltaTime);
    setVelocity(vel);

    // Clamp velocity to max speed
    if (vel.lengthSquared() > maxSpeed_ * maxSpeed_) {
        vel = vel.normalized() * maxSpeed_;
        setVelocity(vel);
    }
}

void Player::render(SDL_Renderer* renderer) {
    Vector2 pos = getPosition();

    // Draw shield if active
    if (shieldActive_ && shieldStrength_ > 0.0f) {
        drawShield(renderer, pos);
    }

    // Draw thrust flames if thrusting
    if (thrusting_) {
        drawThrust(renderer, pos);
    }

    // Draw ship
    drawShip(renderer, pos);
}

void Player::handleInput(float deltaTime) {
    InputSystem& input = InputSystem::getInstance();

    // Reset thrusting state
    thrusting_ = false;

    // Rotation
    if (input.isButtonDown(InputButton::ROTATE_LEFT)) {
        applyRotation(-1.0f, deltaTime);
    }
    if (input.isButtonDown(InputButton::ROTATE_RIGHT)) {
        applyRotation(1.0f, deltaTime);
    }

    // Thrust
    if (input.isButtonDown(InputButton::THRUST)) {
        applyThrust(deltaTime);
        thrusting_ = true;
    }

    // Fire weapon
    if (input.isButtonPressed(InputButton::FIRE)) {
        fire();
    }
}

Vector2 Player::getForward() const {
    return Vector2::fromAngle(rotation_);
}

bool Player::takeDamage(float amount) {
    if (shieldActive_ && shieldStrength_ > 0.0f) {
        // Shield absorbs damage
        shieldStrength_ -= amount;
        timeSinceHit_ = 0.0f;

        if (shieldStrength_ <= 0.0f) {
            shieldStrength_ = 0.0f;
            shieldActive_ = false;
            // TODO: Play shield down sound
        }

        return false; // Not destroyed
    } else {
        // Direct hit - lose a life
        lives_--;

        if (lives_ <= 0) {
            setActive(false);
            return true; // Ship destroyed
        }

        // Respawn with shield
        shieldStrength_ = 1.0f;
        shieldActive_ = true;
        timeSinceHit_ = 0.0f;

        // TODO: Play death sound
        // TODO: Respawn animation

        return false;
    }
}

void Player::restoreShield(float amount) {
    if (shieldActive_) {
        shieldStrength_ += amount;
        if (shieldStrength_ > 1.0f) {
            shieldStrength_ = 1.0f;
        }
    }
}

bool Player::canFire() const {
    return bombCount_ > 0 && timeSinceLastShot_ >= (1.0f / fireRate_);
}

bool Player::fire() {
    if (!canFire()) {
        return false;
    }

    // Fire a sinibomb
    bombCount_--;
    timeSinceLastShot_ = 0.0f;

    // TODO: Create sinibomb projectile
    // TODO: Play fire sound

    return true;
}

void Player::applyThrust(float deltaTime) {
    Vector2 forward = getForward();
    Vector2 thrust = forward * thrustPower_ * deltaTime;

    Vector2 vel = getVelocity();
    vel += thrust;
    setVelocity(vel);
}

void Player::applyRotation(float direction, float deltaTime) {
    rotation_ += direction * rotationSpeed_ * deltaTime;

    // Wrap rotation to 0 to 2PI
    rotation_ = Math::wrapAngle(rotation_);
}

void Player::updateShield(float deltaTime) {
    // Recharge shield after delay
    if (shieldActive_ && shieldStrength_ < 1.0f) {
        if (timeSinceHit_ >= shieldRechargeDelay_) {
            shieldStrength_ += shieldRechargeRate_ * deltaTime;
            if (shieldStrength_ > 1.0f) {
                shieldStrength_ = 1.0f;
            }
        }
    }

    // Reactivate shield if fully charged
    if (!shieldActive_ && shieldStrength_ >= 1.0f) {
        shieldActive_ = true;
    }
}

void Player::updateWeapons(float deltaTime) {
    timeSinceLastShot_ += deltaTime;
}

void Player::drawShip(SDL_Renderer* renderer, const Vector2& pos) {
    // Draw ship as a triangle pointing in rotation direction
    // Original arcade used a simple triangular ship

    Vector2 forward = getForward();
    Vector2 right = forward.perpendicular();

    // Ship vertices (triangle)
    Vector2 nose = pos + forward * shipSize_;
    Vector2 leftWing = pos - forward * (shipSize_ * 0.5f) - right * (shipSize_ * 0.6f);
    Vector2 rightWing = pos - forward * (shipSize_ * 0.5f) + right * (shipSize_ * 0.6f);

    // Draw filled triangle
    SDL_SetRenderDrawColor(renderer, shipColor_.r, shipColor_.g, shipColor_.b, shipColor_.a);

    // Draw triangle using lines (SDL doesn't have filled triangle primitive)
    SDL_RenderDrawLine(renderer,
        static_cast<int>(nose.x), static_cast<int>(nose.y),
        static_cast<int>(leftWing.x), static_cast<int>(leftWing.y));
    SDL_RenderDrawLine(renderer,
        static_cast<int>(leftWing.x), static_cast<int>(leftWing.y),
        static_cast<int>(rightWing.x), static_cast<int>(rightWing.y));
    SDL_RenderDrawLine(renderer,
        static_cast<int>(rightWing.x), static_cast<int>(rightWing.y),
        static_cast<int>(nose.x), static_cast<int>(nose.y));

    // Draw center line for detail
    SDL_RenderDrawLine(renderer,
        static_cast<int>(pos.x), static_cast<int>(pos.y),
        static_cast<int>(nose.x), static_cast<int>(nose.y));
}

void Player::drawThrust(SDL_Renderer* renderer, const Vector2& pos) {
    // Draw thrust flames behind ship
    Vector2 forward = getForward();
    Vector2 right = forward.perpendicular();

    // Animate thrust flames
    float flameLength = shipSize_ * (0.8f + 0.2f * std::sin(thrustTimer_ * 20.0f));

    Vector2 thrustStart = pos - forward * (shipSize_ * 0.3f);
    Vector2 thrustEnd = pos - forward * (shipSize_ * 0.3f + flameLength);
    Vector2 thrustLeft = thrustEnd - right * (shipSize_ * 0.3f);
    Vector2 thrustRight = thrustEnd + right * (shipSize_ * 0.3f);

    // Draw thrust as orange/red lines
    SDL_SetRenderDrawColor(renderer, thrustColor_.r, thrustColor_.g, thrustColor_.b, thrustColor_.a);

    SDL_RenderDrawLine(renderer,
        static_cast<int>(thrustStart.x), static_cast<int>(thrustStart.y),
        static_cast<int>(thrustLeft.x), static_cast<int>(thrustLeft.y));
    SDL_RenderDrawLine(renderer,
        static_cast<int>(thrustStart.x), static_cast<int>(thrustStart.y),
        static_cast<int>(thrustEnd.x), static_cast<int>(thrustEnd.y));
    SDL_RenderDrawLine(renderer,
        static_cast<int>(thrustStart.x), static_cast<int>(thrustStart.y),
        static_cast<int>(thrustRight.x), static_cast<int>(thrustRight.y));
}

void Player::drawShield(SDL_Renderer* renderer, const Vector2& pos) {
    // Draw shield as a circle around the ship
    // Opacity based on shield strength
    float alpha = shieldStrength_ * shieldColor_.a;
    SDL_SetRenderDrawColor(renderer, shieldColor_.r, shieldColor_.g, shieldColor_.b,
                           static_cast<Uint8>(alpha));

    // Draw shield circle (octagon approximation)
    const int segments = 16;
    float radius = shipSize_ * 1.5f;

    for (int i = 0; i < segments; i++) {
        float angle1 = (Math::TWO_PI / segments) * i;
        float angle2 = (Math::TWO_PI / segments) * ((i + 1) % segments);

        Vector2 p1 = pos + Vector2::fromAngle(angle1, radius);
        Vector2 p2 = pos + Vector2::fromAngle(angle2, radius);

        SDL_RenderDrawLine(renderer,
            static_cast<int>(p1.x), static_cast<int>(p1.y),
            static_cast<int>(p2.x), static_cast<int>(p2.y));
    }
}
