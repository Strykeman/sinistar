/**
 * Math.cpp
 *
 * Implementation of math utilities
 */

#include "Math.h"

// Vector2 static constants
const Vector2 Vector2::ZERO = Vector2(0.0f, 0.0f);
const Vector2 Vector2::ONE = Vector2(1.0f, 1.0f);
const Vector2 Vector2::UP = Vector2(0.0f, -1.0f);     // Y axis points down in screen coords
const Vector2 Vector2::DOWN = Vector2(0.0f, 1.0f);
const Vector2 Vector2::LEFT = Vector2(-1.0f, 0.0f);
const Vector2 Vector2::RIGHT = Vector2(1.0f, 0.0f);

// MathUtils implementation

float MathUtils::distance(const Vector2& a, const Vector2& b) {
    return (b - a).length();
}

float MathUtils::distanceSquared(const Vector2& a, const Vector2& b) {
    return (b - a).lengthSquared();
}

float MathUtils::angleBetween(const Vector2& from, const Vector2& to) {
    return std::atan2(to.y - from.y, to.x - from.x);
}

float MathUtils::angleDifference(float from, float to) {
    float diff = to - from;
    return Math::wrapAngle(diff);
}

Vector2 MathUtils::rotateAround(const Vector2& point, const Vector2& origin, float angle) {
    Vector2 relative = point - origin;
    return origin + relative.rotated(angle);
}

bool MathUtils::pointInCircle(const Vector2& point, const Vector2& center, float radius) {
    return distanceSquared(point, center) <= radius * radius;
}

bool MathUtils::circlesOverlap(const Vector2& center1, float radius1,
                               const Vector2& center2, float radius2) {
    float totalRadius = radius1 + radius2;
    return distanceSquared(center1, center2) <= totalRadius * totalRadius;
}

Vector2 MathUtils::wrapToScreen(const Vector2& pos, float width, float height) {
    Vector2 wrapped = pos;

    // Wrap X coordinate
    while (wrapped.x < 0.0f) {
        wrapped.x += width;
    }
    while (wrapped.x >= width) {
        wrapped.x -= width;
    }

    // Wrap Y coordinate
    while (wrapped.y < 0.0f) {
        wrapped.y += height;
    }
    while (wrapped.y >= height) {
        wrapped.y -= height;
    }

    return wrapped;
}

Vector2 MathUtils::moveTowards(const Vector2& current, const Vector2& target, float maxStep) {
    Vector2 diff = target - current;
    float dist = diff.length();

    if (dist <= maxStep || dist < Math::EPSILON) {
        return target;
    }

    return current + diff * (maxStep / dist);
}

float MathUtils::smoothDampFloat(float current, float target, float& velocity,
                                 float smoothTime, float deltaTime, float maxSpeed) {
    smoothTime = std::max(0.0001f, smoothTime);
    float omega = 2.0f / smoothTime;
    float x = omega * deltaTime;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);

    float change = current - target;
    float originalTo = target;

    // Clamp maximum speed
    float maxChange = maxSpeed * smoothTime;
    change = Math::clamp(change, -maxChange, maxChange);
    target = current - change;

    float temp = (velocity + omega * change) * deltaTime;
    velocity = (velocity - omega * temp) * exp;
    float output = target + (change + temp) * exp;

    // Prevent overshooting
    if ((originalTo - current > 0.0f) == (output > originalTo)) {
        output = originalTo;
        velocity = (output - originalTo) / deltaTime;
    }

    return output;
}

Vector2 MathUtils::smoothDamp(const Vector2& current, const Vector2& target,
                              Vector2& velocity, float smoothTime, float deltaTime,
                              float maxSpeed) {
    return Vector2(
        smoothDampFloat(current.x, target.x, velocity.x, smoothTime, deltaTime, maxSpeed),
        smoothDampFloat(current.y, target.y, velocity.y, smoothTime, deltaTime, maxSpeed)
    );
}
