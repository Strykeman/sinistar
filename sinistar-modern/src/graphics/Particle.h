/**
 * Particle.h
 *
 * Particle system for explosions and visual effects
 */

#ifndef PARTICLE_H
#define PARTICLE_H

#include "../core/Math.h"
#include <SDL.h>
#include <vector>

/**
 * Particle - single particle for effects
 */
struct Particle {
    Vector2 position;
    Vector2 velocity;
    SDL_Color color;
    float lifetime;
    float maxLifetime;
    float size;
    bool active;

    Particle()
        : position(0, 0)
        , velocity(0, 0)
        , color{255, 255, 255, 255}
        , lifetime(1.0f)
        , maxLifetime(1.0f)
        , size(2.0f)
        , active(true)
    {}
};

/**
 * ParticleSystem - manages particle effects
 */
class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();

    /**
     * Update all particles
     */
    void update(float deltaTime);

    /**
     * Render all particles
     */
    void render(SDL_Renderer* renderer);

    /**
     * Create explosion effect
     */
    void createExplosion(const Vector2& position, const SDL_Color& color, int particleCount = 20);

    /**
     * Create impact effect
     */
    void createImpact(const Vector2& position, const Vector2& direction, const SDL_Color& color);

    /**
     * Clear all particles
     */
    void clear();

    /**
     * Get active particle count
     */
    int getParticleCount() const;

private:
    std::vector<Particle> particles_;
    const int MAX_PARTICLES = 1000;
};

#endif // PARTICLE_H
