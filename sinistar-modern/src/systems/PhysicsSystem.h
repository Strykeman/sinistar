/**
 * PhysicsSystem.h
 *
 * Physics system - handles velocity, acceleration, collision response
 * Based on original VELOCITY.SRC and BOUNCE.SRC algorithms
 */

#ifndef PHYSICSSYSTEM_H
#define PHYSICSSYSTEM_H

#include "../core/Math.h"
#include <vector>

class GameObject;

/**
 * PhysicsSystem - handles all physics calculations
 * Replicates velocity and bounce systems from original game
 */
class PhysicsSystem {
public:
    /**
     * Constructor
     * @param worldWidth Width of game world for wrapping
     * @param worldHeight Height of game world for wrapping
     */
    PhysicsSystem(float worldWidth, float worldHeight);

    /**
     * Destructor
     */
    ~PhysicsSystem();

    /**
     * Update physics for all registered objects
     * @param deltaTime Time since last update in seconds
     */
    void update(float deltaTime);

    /**
     * Register an object for physics updates
     * @param object GameObject to register
     */
    void registerObject(GameObject* object);

    /**
     * Unregister an object
     * @param object GameObject to unregister
     */
    void unregisterObject(GameObject* object);

    /**
     * Clear all registered objects
     */
    void clear();

    /**
     * Apply velocity to update position
     * (Replicates UPDLVELOCITY/UPDSVELOCITY from VELOCITY.SRC)
     * @param object GameObject to update
     * @param deltaTime Time step
     */
    static void applyVelocity(GameObject* object, float deltaTime);

    /**
     * Apply acceleration to velocity
     * @param velocity Current velocity
     * @param acceleration Acceleration to apply
     * @param maxSpeed Maximum speed limit
     * @param deltaTime Time step
     * @return New velocity
     */
    static Vector2 applyAcceleration(const Vector2& velocity,
                                     const Vector2& acceleration,
                                     float maxSpeed,
                                     float deltaTime);

    /**
     * Apply drag/friction to velocity
     * @param velocity Current velocity
     * @param drag Drag coefficient (0.0 = no drag, 1.0 = instant stop)
     * @param deltaTime Time step
     * @return New velocity with drag applied
     */
    static Vector2 applyDrag(const Vector2& velocity, float drag, float deltaTime);

    /**
     * Calculate desired velocity based on distance to target
     * (Replicates NEWVELOCITY from VELOCITY.SRC)
     * @param distance Distance to target
     * @param maxSpeed Maximum speed
     * @param minDistance Minimum distance to target
     * @return Desired velocity magnitude
     */
    static float calculateDesiredVelocity(float distance, float maxSpeed, float minDistance = 10.0f);

    /**
     * Bounce two objects off each other with momentum exchange
     * (Replicates BOUNCE routine from BOUNCE.SRC)
     * @param obj1 First object
     * @param obj2 Second object
     * @param mass1 Mass of first object
     * @param mass2 Mass of second object
     */
    static void bounceObjects(GameObject* obj1, GameObject* obj2, float mass1, float mass2);

    /**
     * Wrap object position to world boundaries
     * @param object GameObject to wrap
     * @param worldWidth World width
     * @param worldHeight World height
     */
    static void wrapToWorld(GameObject* object, float worldWidth, float worldHeight);

    /**
     * Get world dimensions
     */
    float getWorldWidth() const { return worldWidth_; }
    float getWorldHeight() const { return worldHeight_; }

    /**
     * Set world dimensions
     */
    void setWorldSize(float width, float height) {
        worldWidth_ = width;
        worldHeight_ = height;
    }

private:
    std::vector<GameObject*> objects_;
    float worldWidth_;
    float worldHeight_;
};

#endif // PHYSICSSYSTEM_H
