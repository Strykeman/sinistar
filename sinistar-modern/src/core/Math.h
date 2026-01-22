/**
 * Math.h
 *
 * Math utilities for game physics and calculations
 * Based on original DISTANCE.SRC, VELOCITY.SRC algorithms
 */

#ifndef MATH_H
#define MATH_H

#include <cmath>
#include <algorithm>

// Mathematical constants
namespace Math {
    constexpr float PI = 3.14159265358979323846f;
    constexpr float TWO_PI = PI * 2.0f;
    constexpr float HALF_PI = PI * 0.5f;
    constexpr float DEG_TO_RAD = PI / 180.0f;
    constexpr float RAD_TO_DEG = 180.0f / PI;

    // Small epsilon for floating point comparisons
    constexpr float EPSILON = 0.00001f;

    /**
     * Clamp value between min and max
     */
    template<typename T>
    inline T clamp(T value, T min, T max) {
        return std::max(min, std::min(value, max));
    }

    /**
     * Linear interpolation
     */
    inline float lerp(float a, float b, float t) {
        return a + (b - a) * t;
    }

    /**
     * Wrap angle to [-PI, PI] range
     */
    inline float wrapAngle(float angle) {
        while (angle > PI) angle -= TWO_PI;
        while (angle < -PI) angle += TWO_PI;
        return angle;
    }

    /**
     * Check if two floats are approximately equal
     */
    inline bool approxEqual(float a, float b, float epsilon = EPSILON) {
        return std::abs(a - b) < epsilon;
    }
}

/**
 * Vector2 - 2D vector for position, velocity, direction
 * Enhanced from GameObject.h with full math operations
 */
struct Vector2 {
    float x;
    float y;

    // Constructors
    Vector2() : x(0.0f), y(0.0f) {}
    Vector2(float x_, float y_) : x(x_), y(y_) {}

    // Basic operations
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 operator/(float scalar) const {
        return Vector2(x / scalar, y / scalar);
    }

    Vector2 operator-() const {
        return Vector2(-x, -y);
    }

    // Compound assignment operators
    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    Vector2& operator/=(float scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Comparison operators
    bool operator==(const Vector2& other) const {
        return Math::approxEqual(x, other.x) && Math::approxEqual(y, other.y);
    }

    bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }

    // Vector operations
    float length() const {
        return std::sqrt(x * x + y * y);
    }

    float lengthSquared() const {
        return x * x + y * y;
    }

    Vector2 normalized() const {
        float len = length();
        if (len > Math::EPSILON) {
            return Vector2(x / len, y / len);
        }
        return Vector2(0.0f, 0.0f);
    }

    void normalize() {
        float len = length();
        if (len > Math::EPSILON) {
            x /= len;
            y /= len;
        } else {
            x = 0.0f;
            y = 0.0f;
        }
    }

    // Dot product
    float dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    // Cross product (returns scalar in 2D)
    float cross(const Vector2& other) const {
        return x * other.y - y * other.x;
    }

    // Distance to another vector
    float distanceTo(const Vector2& other) const {
        return (*this - other).length();
    }

    float distanceSquaredTo(const Vector2& other) const {
        return (*this - other).lengthSquared();
    }

    // Angle in radians (from positive X axis)
    float angle() const {
        return std::atan2(y, x);
    }

    // Angle to another vector
    float angleTo(const Vector2& other) const {
        return std::atan2(other.y - y, other.x - x);
    }

    // Create vector from angle and length
    static Vector2 fromAngle(float angle, float length = 1.0f) {
        return Vector2(std::cos(angle) * length, std::sin(angle) * length);
    }

    // Rotate vector by angle (radians)
    Vector2 rotated(float angle) const {
        float cosAngle = std::cos(angle);
        float sinAngle = std::sin(angle);
        return Vector2(
            x * cosAngle - y * sinAngle,
            x * sinAngle + y * cosAngle
        );
    }

    void rotate(float angle) {
        float cosAngle = std::cos(angle);
        float sinAngle = std::sin(angle);
        float newX = x * cosAngle - y * sinAngle;
        float newY = x * sinAngle + y * cosAngle;
        x = newX;
        y = newY;
    }

    // Linear interpolation
    Vector2 lerp(const Vector2& target, float t) const {
        return Vector2(
            Math::lerp(x, target.x, t),
            Math::lerp(y, target.y, t)
        );
    }

    // Clamp vector length
    Vector2 clamped(float maxLength) const {
        float len = length();
        if (len > maxLength) {
            return (*this) * (maxLength / len);
        }
        return *this;
    }

    // Perpendicular vector (rotated 90 degrees)
    Vector2 perpendicular() const {
        return Vector2(-y, x);
    }

    // Reflect vector across normal
    Vector2 reflect(const Vector2& normal) const {
        return *this - normal * (2.0f * dot(normal));
    }

    // Static zero and unit vectors
    static const Vector2 ZERO;
    static const Vector2 ONE;
    static const Vector2 UP;
    static const Vector2 DOWN;
    static const Vector2 LEFT;
    static const Vector2 RIGHT;
};

// Allow scalar * vector multiplication
inline Vector2 operator*(float scalar, const Vector2& vec) {
    return vec * scalar;
}

/**
 * MathUtils - Game-specific math utilities
 * Based on original DISTANCE.SRC algorithms
 */
class MathUtils {
public:
    /**
     * Calculate distance between two points
     * (Replicates GETDISTANCE from DISTANCE.SRC)
     */
    static float distance(const Vector2& a, const Vector2& b);

    /**
     * Calculate squared distance (faster, no sqrt)
     */
    static float distanceSquared(const Vector2& a, const Vector2& b);

    /**
     * Calculate angle from point A to point B in radians
     */
    static float angleBetween(const Vector2& from, const Vector2& to);

    /**
     * Calculate signed angle difference (shortest rotation)
     */
    static float angleDifference(float from, float to);

    /**
     * Rotate a point around an origin
     */
    static Vector2 rotateAround(const Vector2& point, const Vector2& origin, float angle);

    /**
     * Check if point is within a circle
     */
    static bool pointInCircle(const Vector2& point, const Vector2& center, float radius);

    /**
     * Check if two circles overlap
     */
    static bool circlesOverlap(const Vector2& center1, float radius1,
                               const Vector2& center2, float radius2);

    /**
     * Wrap coordinate to screen boundaries
     * (Used for wrapping game world like original)
     */
    static Vector2 wrapToScreen(const Vector2& pos, float width, float height);

    /**
     * Move towards target with maximum step
     */
    static Vector2 moveTowards(const Vector2& current, const Vector2& target, float maxStep);

    /**
     * Smooth damp (spring-based smoothing)
     */
    static Vector2 smoothDamp(const Vector2& current, const Vector2& target,
                             Vector2& velocity, float smoothTime, float deltaTime,
                             float maxSpeed = INFINITY);
private:
    static float smoothDampFloat(float current, float target, float& velocity,
                                 float smoothTime, float deltaTime, float maxSpeed);
};

#endif // MATH_H
