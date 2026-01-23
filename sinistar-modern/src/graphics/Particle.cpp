/**
 * Particle.cpp
 *
 * Implementation of particle system
 */

#include "Particle.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

ParticleSystem::ParticleSystem() {
    particles_.reserve(MAX_PARTICLES);
}

ParticleSystem::~ParticleSystem() {
    clear();
}

void ParticleSystem::update(float deltaTime) {
    // Update all particles
    for (auto& particle : particles_) {
        if (!particle.active) continue;

        // Update position
        particle.position += particle.velocity * deltaTime;

        // Update lifetime
        particle.lifetime -= deltaTime;

        // Fade out alpha based on remaining lifetime
        float lifePercent = particle.lifetime / particle.maxLifetime;
        particle.color.a = static_cast<Uint8>(255 * lifePercent);

        // Deactivate if expired
        if (particle.lifetime <= 0.0f) {
            particle.active = false;
        }

        // Apply drag
        particle.velocity *= 0.98f;
    }

    // Remove inactive particles
    particles_.erase(
        std::remove_if(particles_.begin(), particles_.end(),
            [](const Particle& p) { return !p.active; }),
        particles_.end()
    );
}

void ParticleSystem::render(SDL_Renderer* renderer) {
    for (const auto& particle : particles_) {
        if (!particle.active) continue;

        SDL_SetRenderDrawColor(renderer,
            particle.color.r, particle.color.g, particle.color.b, particle.color.a);

        // Draw particle as a small square
        SDL_Rect rect = {
            static_cast<int>(particle.position.x - particle.size / 2),
            static_cast<int>(particle.position.y - particle.size / 2),
            static_cast<int>(particle.size),
            static_cast<int>(particle.size)
        };
        SDL_RenderFillRect(renderer, &rect);
    }
}

void ParticleSystem::createExplosion(const Vector2& position, const SDL_Color& color, int particleCount) {
    if (particles_.size() >= static_cast<size_t>(MAX_PARTICLES)) return;

    for (int i = 0; i < particleCount; i++) {
        Particle p;
        p.position = position;

        // Random direction
        float angle = (rand() % 360) * Math::DEG_TO_RAD;
        float speed = 50.0f + (rand() % 100);
        p.velocity = Vector2::fromAngle(angle, speed);

        // Color with some variation
        p.color = color;
        p.color.r = std::min(255, color.r + (rand() % 50) - 25);
        p.color.g = std::min(255, color.g + (rand() % 50) - 25);
        p.color.b = std::min(255, color.b + (rand() % 50) - 25);

        // Lifetime
        p.lifetime = 0.3f + (rand() % 100) / 200.0f;  // 0.3 - 0.8 seconds
        p.maxLifetime = p.lifetime;

        // Size
        p.size = 2.0f + (rand() % 3);

        p.active = true;

        particles_.push_back(p);
    }
}

void ParticleSystem::createImpact(const Vector2& position, const Vector2& direction, const SDL_Color& color) {
    if (particles_.size() >= static_cast<size_t>(MAX_PARTICLES)) return;

    // Create particles in direction of impact
    for (int i = 0; i < 10; i++) {
        Particle p;
        p.position = position;

        // Direction with some spread
        Vector2 dir = direction.normalized();
        float spread = ((rand() % 100) - 50) / 100.0f;  // -0.5 to 0.5
        Vector2 perpendicular = dir.perpendicular();
        Vector2 spreadDir = (dir + perpendicular * spread).normalized();

        float speed = 80.0f + (rand() % 60);
        p.velocity = spreadDir * speed;

        p.color = color;
        p.lifetime = 0.2f + (rand() % 100) / 500.0f;
        p.maxLifetime = p.lifetime;
        p.size = 2.0f;
        p.active = true;

        particles_.push_back(p);
    }
}

void ParticleSystem::clear() {
    particles_.clear();
}

int ParticleSystem::getParticleCount() const {
    return static_cast<int>(particles_.size());
}
