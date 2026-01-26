/**
 * Crystal.h
 *
 * Crystal planetoid entity - can be mined for resources
 * Core gameplay mechanic from original Sinistar
 */

#ifndef CRYSTAL_H
#define CRYSTAL_H

#include "GameObject.h"
#include <SDL.h>

/**
 * Crystal - planetoid containing crystals
 * Can be shot by player or mined by Workers
 */
class Crystal : public GameObject {
public:
    Crystal();
    virtual ~Crystal();

    /**
     * Update crystal state
     */
    void update(float deltaTime) override;

    /**
     * Render crystal
     */
    void render(SDL_Renderer* renderer);

    /**
     * Take damage from weapon
     * @return true if crystal is destroyed
     */
    bool takeDamage(float amount);

    /**
     * Mine crystal (by worker)
     * @return Amount of crystal mined (0.0 to 1.0)
     */
    float mine(float amount);

    /**
     * Get health
     */
    float getHealth() const { return health_; }

    /**
     * Get radius for collision
     */
    float getRadius() const { return radius_; }

    /**
     * Get crystal value (how much crystal remains)
     */
    float getCrystalAmount() const { return crystalAmount_; }

    /**
     * Check if crystal is depleted
     */
    bool isDepleted() const { return crystalAmount_ <= 0.0f; }

    /**
     * Check if should be removed
     */
    bool shouldRemove() const { return !isActive() || isDepleted(); }

private:
    float health_;           // Hit points (destroyed when 0)
    float maxHealth_;        // Maximum health
    float crystalAmount_;    // Amount of crystal (0.0 to 1.0)
    float radius_;           // Collision radius

    // Visual
    float rotation_;         // Rotation angle
    float rotationSpeed_;    // Rotation speed
    float pulseTimer_;       // For pulsing glow effect

    // Crystal size variant
    enum class Size {
        SMALL,   // 1 crystal
        MEDIUM,  // 3 crystals
        LARGE    // 5 crystals
    };
    Size size_;
};

#endif // CRYSTAL_H
