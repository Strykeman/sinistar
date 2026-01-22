/**
 * PhysicsSystem.cpp
 *
 * Implementation of physics system
 */

#include "PhysicsSystem.h"
#include "../entities/GameObject.h"
#include <algorithm>
#include <iostream>

PhysicsSystem::PhysicsSystem(float worldWidth, float worldHeight)
    : worldWidth_(worldWidth)
    , worldHeight_(worldHeight)
{
    std::cout << "PhysicsSystem initialized (" << worldWidth << "x" << worldHeight << ")" << std::endl;
}

PhysicsSystem::~PhysicsSystem() {
    clear();
    std::cout << "PhysicsSystem destroyed" << std::endl;
}

void PhysicsSystem::update(float deltaTime) {
    // Update physics for all registered objects
    for (GameObject* object : objects_) {
        if (object && object->isActive()) {
            // Apply velocity to position
            applyVelocity(object, deltaTime);

            // Wrap to world boundaries
            wrapToWorld(object, worldWidth_, worldHeight_);
        }
    }
}

void PhysicsSystem::registerObject(GameObject* object) {
    if (object) {
        objects_.push_back(object);
    }
}

void PhysicsSystem::unregisterObject(GameObject* object) {
    objects_.erase(
        std::remove(objects_.begin(), objects_.end(), object),
        objects_.end()
    );
}

void PhysicsSystem::clear() {
    objects_.clear();
}

void PhysicsSystem::applyVelocity(GameObject* object, float deltaTime) {
    // Update position based on velocity
    // position = position + velocity * deltaTime
    Vector2 newPos = object->getPosition() + object->getVelocity() * deltaTime;
    object->setPosition(newPos);
}

Vector2 PhysicsSystem::applyAcceleration(const Vector2& velocity,
                                         const Vector2& acceleration,
                                         float maxSpeed,
                                         float deltaTime) {
    // Update velocity: v = v + a * dt
    Vector2 newVelocity = velocity + acceleration * deltaTime;

    // Clamp to maximum speed
    if (maxSpeed > 0.0f) {
        newVelocity = newVelocity.clamped(maxSpeed);
    }

    return newVelocity;
}

Vector2 PhysicsSystem::applyDrag(const Vector2& velocity, float drag, float deltaTime) {
    // Apply exponential drag: v = v * (1 - drag)^dt
    // Approximated as: v = v * (1 - drag * dt) for small dt
    float dragFactor = 1.0f - Math::clamp(drag * deltaTime, 0.0f, 1.0f);
    return velocity * dragFactor;
}

float PhysicsSystem::calculateDesiredVelocity(float distance, float maxSpeed, float minDistance) {
    // Based on NEWVELOCITY from VELOCITY.SRC
    // Velocity scales with distance, up to maxSpeed

    if (std::abs(distance) < minDistance) {
        // Within minimum distance, slow down
        return 0.0f;
    }

    // Linear scaling: closer to target = slower speed
    // This creates smooth approach behavior
    float normalizedDist = std::abs(distance) / 100.0f; // Arbitrary scale factor
    float speed = Math::clamp(normalizedDist * maxSpeed, 0.0f, maxSpeed);

    // Return signed velocity (negative distance needs positive velocity)
    return (distance < 0) ? speed : -speed;
}

void PhysicsSystem::bounceObjects(GameObject* obj1, GameObject* obj2, float mass1, float mass2) {
    // Based on BOUNCE routine from BOUNCE.SRC
    // Implements momentum-conserving elastic collision

    if (!obj1 || !obj2) return;

    // Get current velocities
    Vector2 v1 = obj1->getVelocity();
    Vector2 v2 = obj2->getVelocity();

    // Handle massless objects (infinite mass collision)
    if (mass1 <= 0.0f) {
        // obj1 is immovable, obj2 bounces off
        obj2->setVelocity(-v2);
        return;
    }
    if (mass2 <= 0.0f) {
        // obj2 is immovable, obj1 bounces off
        obj1->setVelocity(-v1);
        return;
    }

    // Calculate center of momentum
    // COM velocity = (m1*v1 + m2*v2) / (m1 + m2)
    float totalMass = mass1 + mass2;
    Vector2 comVelocity = (v1 * mass1 + v2 * mass2) / totalMass;

    // Calculate velocities relative to COM
    Vector2 relV1 = v1 - comVelocity;
    Vector2 relV2 = v2 - comVelocity;

    // Reverse relative velocities (elastic collision)
    Vector2 newRelV1 = -relV1;
    Vector2 newRelV2 = -relV2;

    // Convert back to world velocities
    Vector2 newV1 = comVelocity + newRelV1;
    Vector2 newV2 = comVelocity + newRelV2;

    // Note: Original game multiplies S axis by 2 to compensate for aspect ratio
    // For now we'll keep it symmetric, but can add aspect correction later if needed

    // Apply new velocities
    obj1->setVelocity(newV1);
    obj2->setVelocity(newV2);
}

void PhysicsSystem::wrapToWorld(GameObject* object, float worldWidth, float worldHeight) {
    Vector2 pos = object->getPosition();
    Vector2 wrapped = MathUtils::wrapToScreen(pos, worldWidth, worldHeight);

    if (wrapped != pos) {
        object->setPosition(wrapped);
    }
}
