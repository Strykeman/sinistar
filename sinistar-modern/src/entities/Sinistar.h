/**
 * Sinistar.h
 *
 * The Sinistar boss - built from crystals, hunts the player
 * "BEWARE, I LIVE!" - "RUN COWARD!"
 */

#ifndef SINISTAR_H
#define SINISTAR_H

#include "GameObject.h"
#include <SDL2/SDL.h>
#include <string>

/**
 * Sinistar construction states
 */
enum class SinistarState {
    BUILDING,      // Under construction (not active yet)
    AWAKENING,     // Just completed, playing wake animation
    HUNTING,       // Actively hunting player
    ATTACKING,     // Close to player, preparing bite
    BITING,        // Bite attack in progress
    DYING,         // Destroyed, death animation
    DEAD           // Completely destroyed
};

/**
 * Sinistar - The Boss
 * Built from crystals, hunts player relentlessly
 */
class Sinistar : public GameObject {
public:
    Sinistar();
    virtual ~Sinistar();

    /**
     * Update Sinistar
     */
    void update(float deltaTime) override;

    /**
     * Render Sinistar
     */
    void render(SDL_Renderer* renderer);

    /**
     * Add crystal to construction
     * @param amount Amount of crystal to add
     * @return true if construction completed
     */
    bool addCrystal(float amount);

    /**
     * Get construction progress (0.0 to 1.0)
     */
    float getConstructionProgress() const;

    /**
     * Check if fully constructed
     */
    bool isConstructed() const { return constructionProgress_ >= 1.0f; }

    /**
     * Activate Sinistar (when construction complete)
     */
    void activate();

    /**
     * Set player target
     */
    void setTarget(GameObject* target) { target_ = target; }

    /**
     * Take damage
     * @return true if destroyed
     */
    bool takeDamage(float amount);

    /**
     * Get current state
     */
    SinistarState getState() const { return state_; }

    /**
     * Get health
     */
    float getHealth() const { return health_; }

    /**
     * Get max health
     */
    float getMaxHealth() const { return maxHealth_; }

    /**
     * Get radius for collision
     */
    float getRadius() const { return radius_; }

    /**
     * Check if should be removed
     */
    bool shouldRemove() const { return state_ == SinistarState::DEAD; }

    /**
     * Get current voice line (if any)
     */
    std::string getVoiceLine() const { return currentVoiceLine_; }

    /**
     * Clear voice line
     */
    void clearVoiceLine() { currentVoiceLine_ = ""; }

private:
    /**
     * Execute current state behavior
     */
    void executeState(float deltaTime);

    /**
     * State behaviors
     */
    void stateBuilding(float deltaTime);
    void stateAwakening(float deltaTime);
    void stateHunting(float deltaTime);
    void stateAttacking(float deltaTime);
    void stateBiting(float deltaTime);
    void stateDying(float deltaTime);

    /**
     * Trigger voice line
     */
    void speak(const std::string& line);

    SinistarState state_;
    GameObject* target_;

    // Construction
    float constructionProgress_;  // 0.0 to 1.0
    float crystalsNeeded_;        // Total crystals needed to complete
    float crystalsCollected_;     // Crystals collected so far

    // Combat stats
    float health_;
    float maxHealth_;
    float thrustPower_;
    float maxSpeed_;
    float radius_;

    // Attack
    float attackRange_;           // Range to start attacking
    float biteRange_;             // Range for bite attack
    float biteDamage_;
    float biteTimer_;
    float biteCooldown_;

    // State timers
    float stateTimer_;
    float awakeningDuration_;

    // Visual
    float rotation_;
    float pulseTimer_;
    float jawAngle_;              // For bite animation
    float scale_;                 // Size scale during awakening

    // Voice
    std::string currentVoiceLine_;
    float voiceTimer_;
    float voiceCooldown_;

    // Death
    float deathTimer_;
    float deathDuration_;
};

#endif // SINISTAR_H
