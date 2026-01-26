/**
 * Sinibomb.h
 *
 * Player's projectile weapon
 * Based on original arcade sinibomb behavior
 */

#ifndef SINIBOMB_H
#define SINIBOMB_H

#include "GameObject.h"
#include <SDL.h>

/**
 * Sinibomb - player's projectile weapon
 * Flies in a straight line, explodes on timeout or impact
 */
class Sinibomb : public GameObject {
public:
    /**
     * Constructor
     * @param position Starting position
     * @param velocity Initial velocity
     * @param owner Pointer to owner (nullptr for enemy bombs)
     */
    Sinibomb(const Vector2& position, const Vector2& velocity, GameObject* owner = nullptr);

    /**
     * Destructor
     */
    ~Sinibomb() override;

    /**
     * Update projectile
     * @param deltaTime Time since last update
     */
    void update(float deltaTime) override;

    /**
     * Render projectile
     * @param renderer SDL renderer
     */
    void render(SDL_Renderer* renderer);

    /**
     * Get owner of this projectile
     */
    GameObject* getOwner() const { return owner_; }

    /**
     * Check if projectile should be removed
     */
    bool shouldRemove() const { return !isActive() || lifetime_ <= 0.0f; }

    /**
     * Explode (deactivate and mark for removal)
     */
    void explode();

    /**
     * Get damage value
     */
    float getDamage() const { return damage_; }

    /**
     * Get collision radius
     */
    float getRadius() const { return radius_; }

private:
    GameObject* owner_;       // Who fired this projectile
    float lifetime_;          // Time remaining before auto-explode
    float maxLifetime_;       // Maximum lifetime
    float damage_;            // Damage on impact
    float radius_;            // Collision radius
    SDL_Color color_;         // Projectile color
    float pulseTimer_;        // For visual pulsing effect
};

#endif // SINIBOMB_H
