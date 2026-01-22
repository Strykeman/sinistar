/**
 * GameObject.cpp
 *
 * Implementation of base GameObject class
 */

#include "GameObject.h"

// GameObject methods
GameObject::GameObject()
    : position_(0.0f, 0.0f)
    , velocity_(0.0f, 0.0f)
    , character_(0)
    , status_(0)
    , active_(true)
{
}

GameObject::~GameObject() {
}

void GameObject::update(float deltaTime) {
    // Basic physics - update position based on velocity
    position_ = position_ + velocity_ * deltaTime;

    // TODO: Add screen wrapping like original game
    // TODO: Add bounds checking
}
