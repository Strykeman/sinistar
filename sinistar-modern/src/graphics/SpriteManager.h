/**
 * SpriteManager.h
 *
 * Manages loading, caching, and retrieving sprite textures
 * Singleton pattern for global access
 */

#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H

#include <SDL.h>
#include <map>
#include <string>

/**
 * SpriteManager - Central sprite texture management system
 *
 * Responsibilities:
 * - Load PNG sprites from assets/sprites/ directory
 * - Cache SDL_Texture* instances to avoid redundant loads
 * - Provide sprite retrieval by name
 * - Handle sprite sheets with multiple frames
 * - Graceful fallback when sprites missing
 */
class SpriteManager {
public:
    // Singleton access
    static SpriteManager& getInstance();

    // Initialize with SDL renderer (must be called after SDL_CreateRenderer)
    bool initialize(SDL_Renderer* renderer);

    // Shutdown and cleanup all textures
    void shutdown();

    // Load a sprite from file (cached after first load)
    SDL_Texture* loadSprite(const std::string& filename);

    // Get a cached sprite by name (returns nullptr if not loaded)
    SDL_Texture* getSprite(const std::string& name) const;

    // Check if sprite exists
    bool hasSprite(const std::string& name) const;

    // Get sprite dimensions
    bool getSpriteDimensions(const std::string& name, int& width, int& height) const;

    // Preload commonly used sprites
    void preloadSprites();

    // Check if system is initialized
    bool isInitialized() const { return renderer_ != nullptr; }

private:
    // Private constructor (singleton)
    SpriteManager();
    ~SpriteManager();

    // Prevent copying
    SpriteManager(const SpriteManager&) = delete;
    SpriteManager& operator=(const SpriteManager&) = delete;

    // Helper: Load texture from file
    SDL_Texture* loadTextureFromFile(const std::string& filepath);

    // SDL renderer reference
    SDL_Renderer* renderer_;

    // Sprite cache (name -> texture)
    std::map<std::string, SDL_Texture*> sprites_;

    // Sprite dimensions cache (name -> width, height)
    std::map<std::string, std::pair<int, int>> dimensions_;
};

#endif // SPRITEMANAGER_H
