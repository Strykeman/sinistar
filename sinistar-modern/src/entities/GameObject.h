/**
 * GameObject.h
 *
 * Base class for all game entities (Player, Warriors, Sinistar, etc.)
 * Replicates the object workspace structure from the original game
 * with standardized offsets (OSPOS, OLPOS, OSVEL, OLVEL, etc.)
 */

#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <cstdint>
#include "../core/Math.h"

/**
 * GameObject - base class for all game entities
 *
 * Maps to the original object workspace structure:
 * - OSPOS/OLPOS: Small/Large position (we use float for simplicity)
 * - OSVEL/OLVEL: Small/Large velocity
 * - OCHAR: Character/sprite ID
 * - OSTATUS: Object status flags
 */
class GameObject {
public:
    GameObject();
    virtual ~GameObject();

    /**
     * Update object state
     * @param deltaTime Time since last update in seconds
     */
    virtual void update(float deltaTime);

    /**
     * Render object (override in derived classes)
     */
    virtual void render() {}

    // Position accessors
    const Vector2& getPosition() const { return position_; }
    void setPosition(const Vector2& pos) { position_ = pos; }
    void setPosition(float x, float y) { position_ = Vector2(x, y); }

    // Velocity accessors
    const Vector2& getVelocity() const { return velocity_; }
    void setVelocity(const Vector2& vel) { velocity_ = vel; }
    void setVelocity(float x, float y) { velocity_ = Vector2(x, y); }

    // Status and character
    uint32_t getStatus() const { return status_; }
    void setStatus(uint32_t status) { status_ = status; }

    int getCharacter() const { return character_; }
    void setCharacter(int character) { character_ = character; }

    // Active state
    bool isActive() const { return active_; }
    void setActive(bool active) { active_ = active; }

protected:
    // Position and velocity (replicates OSPOS, OLPOS, OSVEL, OLVEL)
    Vector2 position_;
    Vector2 velocity_;

    // Object properties (replicates OCHAR, OSTATUS)
    int character_;      // Sprite/character ID
    uint32_t status_;    // Status flags

    // Runtime state
    bool active_;        // Is object active?
};

#endif // GAMEOBJECT_H
