/**
 * GameObject.cpp
 *
 * Implementation of base GameObject class
 */

#include "GameObject.h"
#include <cmath>

// Vector2 methods
float Vector2::length() const {
    return std::sqrt(x * x + y * y);
}

float Vector2::lengthSquared() const {
    return x * x + y * y;
}

Vector2 Vector2::normalized() const {
    float len = length();
    if (len > 0.0f) {
        return Vector2(x / len, y / len);
    }
    return Vector2(0.0f, 0.0f);
}

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
