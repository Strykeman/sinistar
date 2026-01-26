/**
 * Player.h
 *
 * Player ship entity
 * Controllable ship with thrust, rotation, shields, and weapons
 */

#ifndef PLAYER_H
#define PLAYER_H

#include "GameObject.h"
#include <SDL.h>

class InputSystem;

/**
 * Player - the player's ship
 * Based on original player ship behavior from the arcade game
 */
class Player : public GameObject {
public:
    /**
     * Constructor
     */
    Player();

    /**
     * Destructor
     */
    ~Player() override;

    /**
     * Update player ship
     * @param deltaTime Time since last update
     */
    void update(float deltaTime) override;

    /**
     * Render player ship
     * @param renderer SDL renderer
     */
    void render(SDL_Renderer* renderer);

    /**
     * Handle input and update controls
     * @param deltaTime Time since last update
     */
    void handleInput(float deltaTime);

    /**
     * Get ship rotation in radians
     */
    float getRotation() const { return rotation_; }

    /**
     * Set ship rotation
     */
    void setRotation(float rotation) { rotation_ = rotation; }

    /**
     * Get forward direction vector
     */
    Vector2 getForward() const;

    /**
     * Check if thrusting
     */
    bool isThrusting() const { return thrusting_; }

    /**
     * Check if shields are active
     */
    bool hasShield() const { return shieldActive_; }

    /**
     * Get shield strength (0.0 to 1.0)
     */
    float getShieldStrength() const { return shieldStrength_; }

    /**
     * Take damage
     * @param amount Damage amount
     * @return true if ship destroyed
     */
    bool takeDamage(float amount);

    /**
     * Restore shield
     * @param amount Amount to restore
     */
    void restoreShield(float amount);

    /**
     * Get number of lives remaining
     */
    int getLives() const { return lives_; }

    /**
     * Set lives
     */
    void setLives(int lives) { lives_ = lives; }

    /**
     * Get bomb count
     */
    int getBombCount() const { return bombCount_; }

    /**
     * Add bombs
     */
    void addBombs(int count) { bombCount_ += count; }

    /**
     * Can fire weapon?
     */
    bool canFire() const;

    /**
     * Fire weapon
     * @return true if weapon fired
     */
    bool fire();

    /**
     * Create a sinibomb projectile
     * @return Sinibomb projectile (caller takes ownership)
     */
    class Sinibomb* createSinibomb();

private:
    // Ship properties
    float rotation_;              // Current rotation in radians
    float rotationSpeed_;         // Rotation speed (rad/sec)
    float thrustPower_;           // Thrust acceleration
    float maxSpeed_;              // Maximum velocity
    float drag_;                  // Velocity drag coefficient

    // Ship state
    bool thrusting_;              // Currently thrusting?
    float thrustTimer_;           // For thrust animation

    // Shield system
    bool shieldActive_;           // Shield currently active?
    float shieldStrength_;        // Shield health (0.0 to 1.0)
    float shieldRechargeRate_;    // Shield recharge per second
    float shieldRechargeDelay_;   // Delay before recharge starts
    float timeSinceHit_;          // Time since last hit

    // Weapon system
    int bombCount_;               // Number of sinibombs available
    float fireRate_;              // Shots per second
    float timeSinceLastShot_;     // Time since last shot

    // Player stats
    int lives_;                   // Lives remaining

    // Visual properties
    float shipSize_;              // Ship visual size
    SDL_Color shipColor_;         // Ship color
    SDL_Color thrustColor_;       // Thrust flame color
    SDL_Color shieldColor_;       // Shield color

    /**
     * Apply thrust in forward direction
     */
    void applyThrust(float deltaTime);

    /**
     * Apply rotation
     */
    void applyRotation(float direction, float deltaTime);

    /**
     * Update shield system
     */
    void updateShield(float deltaTime);

    /**
     * Update weapon cooldown
     */
    void updateWeapons(float deltaTime);

    /**
     * Draw ship body
     */
    void drawShip(SDL_Renderer* renderer, const Vector2& pos);

    /**
     * Draw thrust flames
     */
    void drawThrust(SDL_Renderer* renderer, const Vector2& pos);

    /**
     * Draw shield
     */
    void drawShield(SDL_Renderer* renderer, const Vector2& pos);
};

#endif // PLAYER_H
