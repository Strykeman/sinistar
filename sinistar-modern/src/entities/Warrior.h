/**
 * Warrior.h
 *
 * Enemy warrior ship
 * Based on original WARRIOR.SRC AI behavior
 */

#ifndef WARRIOR_H
#define WARRIOR_H

#include "GameObject.h"
#include <SDL2/SDL.h>

class Player;

/**
 * Warrior AI mission types
 * From original arcade game
 */
enum class WarriorMission {
    ATTACK,      // Attack player directly
    CHASE,       // Chase player with formation
    INTERCEPT,   // Intercept player's path
    DRIFT,       // Drift randomly
    MINING,      // Mine crystals (not implemented yet)
    COUNT
};

/**
 * Warrior - enemy ship
 * Implements AI missions from original game
 */
class Warrior : public GameObject {
public:
    /**
     * Constructor
     * @param mission AI mission type
     */
    explicit Warrior(WarriorMission mission = WarriorMission::ATTACK);

    /**
     * Destructor
     */
    ~Warrior() override;

    /**
     * Update warrior AI and physics
     * @param deltaTime Time since last update
     */
    void update(float deltaTime) override;

    /**
     * Render warrior
     * @param renderer SDL renderer
     */
    void render(SDL_Renderer* renderer);

    /**
     * Set target (usually player)
     */
    void setTarget(GameObject* target) { target_ = target; }

    /**
     * Get target
     */
    GameObject* getTarget() const { return target_; }

    /**
     * Get mission type
     */
    WarriorMission getMission() const { return mission_; }

    /**
     * Set mission type
     */
    void setMission(WarriorMission mission) { mission_ = mission; }

    /**
     * Take damage
     * @param amount Damage amount
     * @return true if destroyed
     */
    bool takeDamage(float amount);

    /**
     * Get health
     */
    float getHealth() const { return health_; }

    /**
     * Get collision radius
     */
    float getRadius() const { return radius_; }

    /**
     * Check if should be removed
     */
    bool shouldRemove() const { return !isActive() || health_ <= 0.0f; }

private:
    /**
     * Execute current AI mission
     */
    void executeMission(float deltaTime);

    /**
     * Attack mission - direct attack on target
     */
    void missionAttack(float deltaTime);

    /**
     * Chase mission - chase target with evasion
     */
    void missionChase(float deltaTime);

    /**
     * Intercept mission - predict and intercept target
     */
    void missionIntercept(float deltaTime);

    /**
     * Drift mission - random movement
     */
    void missionDrift(float deltaTime);

    /**
     * Apply AI-calculated thrust
     */
    void applyThrust(const Vector2& direction, float deltaTime);

    // AI properties
    GameObject* target_;      // Current target (player)
    WarriorMission mission_;  // Current mission
    float aiThinkTimer_;      // Time between AI decisions
    float aiThinkInterval_;   // How often to recalculate AI

    // Physics properties
    float thrustPower_;       // Thrust acceleration
    float maxSpeed_;          // Maximum velocity
    float turnSpeed_;         // How fast it can change direction
    float drag_;              // Velocity drag

    // Combat properties
    float health_;            // Health points
    float maxHealth_;         // Maximum health
    float radius_;            // Collision radius

    // Visual properties
    float size_;              // Visual size
    float rotation_;          // Current rotation for rendering
    SDL_Color color_;         // Ship color

    // Drift mission state
    Vector2 driftTarget_;     // Target position for drift
    float driftTimer_;        // Time until new drift target
};

#endif // WARRIOR_H
