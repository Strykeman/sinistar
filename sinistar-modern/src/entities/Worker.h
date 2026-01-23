/**
 * Worker.h
 *
 * Worker enemy that mines crystals to build Sinistar
 * Different from Warrior - focused on mining, not attacking
 */

#ifndef WORKER_H
#define WORKER_H

#include "GameObject.h"
#include "Crystal.h"
#include <SDL2/SDL.h>

/**
 * Worker AI states
 */
enum class WorkerState {
    SEARCHING,    // Looking for crystals
    MINING,       // Mining a crystal
    RETURNING,    // Returning with crystal to Sinistar
    FLEEING       // Running from player
};

/**
 * Worker - enemy that mines crystals
 */
class Worker : public GameObject {
public:
    Worker();
    virtual ~Worker();

    /**
     * Update worker
     */
    void update(float deltaTime) override;

    /**
     * Render worker
     */
    void render(SDL_Renderer* renderer);

    /**
     * Set target crystal
     */
    void setTargetCrystal(Crystal* crystal) { targetCrystal_ = crystal; }

    /**
     * Set player reference (to flee from)
     */
    void setPlayer(GameObject* player) { player_ = player; }

    /**
     * Take damage
     * @return true if destroyed
     */
    bool takeDamage(float amount);

    /**
     * Get current state
     */
    WorkerState getState() const { return state_; }

    /**
     * Get radius for collision
     */
    float getRadius() const { return radius_; }

    /**
     * Get health
     */
    float getHealth() const { return health_; }

    /**
     * Check if carrying crystal
     */
    bool isCarryingCrystal() const { return carryingCrystal_ > 0.0f; }

    /**
     * Get amount of crystal carrying
     */
    float getCrystalAmount() const { return carryingCrystal_; }

    /**
     * Drop crystal (when destroyed)
     */
    float dropCrystal() {
        float amount = carryingCrystal_;
        carryingCrystal_ = 0.0f;
        return amount;
    }

    /**
     * Check if should be removed
     */
    bool shouldRemove() const { return !isActive(); }

private:
    /**
     * Execute current state behavior
     */
    void executeState(float deltaTime);

    /**
     * State behaviors
     */
    void stateSearching(float deltaTime);
    void stateMining(float deltaTime);
    void stateReturning(float deltaTime);
    void stateFleeing(float deltaTime);

    /**
     * Check if player is too close
     */
    bool isPlayerThreat() const;

    WorkerState state_;
    Crystal* targetCrystal_;
    GameObject* player_;

    // Stats
    float health_;
    float maxHealth_;
    float thrustPower_;
    float maxSpeed_;
    float radius_;

    // Mining
    float miningTimer_;
    float miningRate_;        // Crystals per second
    float carryingCrystal_;   // Amount of crystal being carried
    float maxCrystalCarry_;

    // AI
    float stateTimer_;
    Vector2 returnTarget_;    // Where to return crystals

    // Visual
    float rotation_;
    float pulseTimer_;
};

#endif // WORKER_H
