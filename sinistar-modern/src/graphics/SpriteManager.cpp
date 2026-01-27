/**
 * SpriteManager.cpp
 *
 * Implementation of sprite loading and caching system
 */

#include "SpriteManager.h"
#include <iostream>

// SDL2_image support (optional, falls back to SDL_LoadBMP)
#ifdef HAVE_SDL2_IMAGE
#include <SDL_image.h>
#endif

SpriteManager& SpriteManager::getInstance() {
    static SpriteManager instance;
    return instance;
}

SpriteManager::SpriteManager()
    : renderer_(nullptr)
{
}

SpriteManager::~SpriteManager() {
    shutdown();
}

bool SpriteManager::initialize(SDL_Renderer* renderer) {
    if (!renderer) {
        std::cerr << "SpriteManager: Cannot initialize with null renderer" << std::endl;
        return false;
    }

    renderer_ = renderer;

#ifdef HAVE_SDL2_IMAGE
    // Initialize SDL2_image (PNG support)
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::cerr << "SpriteManager: SDL2_image could not initialize! IMG_Error: "
                  << IMG_GetError() << std::endl;
        std::cerr << "SpriteManager: Falling back to SDL_LoadBMP only" << std::endl;
    } else {
        std::cout << "SpriteManager: SDL2_image initialized (PNG support enabled)" << std::endl;
    }
#else
    std::cout << "SpriteManager: SDL2_image not available, using SDL_LoadBMP only" << std::endl;
#endif

    std::cout << "SpriteManager initialized" << std::endl;
    return true;
}

void SpriteManager::shutdown() {
    // Free all cached textures
    for (auto& pair : sprites_) {
        if (pair.second) {
            SDL_DestroyTexture(pair.second);
        }
    }
    sprites_.clear();
    dimensions_.clear();

#ifdef HAVE_SDL2_IMAGE
    IMG_Quit();
#endif

    renderer_ = nullptr;
    std::cout << "SpriteManager destroyed" << std::endl;
}

SDL_Texture* SpriteManager::loadSprite(const std::string& filename) {
    if (!renderer_) {
        std::cerr << "SpriteManager: Cannot load sprite, not initialized" << std::endl;
        return nullptr;
    }

    // Check if already cached
    auto it = sprites_.find(filename);
    if (it != sprites_.end()) {
        return it->second;
    }

    // Construct file path
    std::string filepath = "assets/sprites/" + filename;

    // Load texture
    SDL_Texture* texture = loadTextureFromFile(filepath);

    if (texture) {
        // Cache the texture
        sprites_[filename] = texture;

        // Query and cache dimensions
        int width, height;
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
        dimensions_[filename] = std::make_pair(width, height);

        std::cout << "SpriteManager: Loaded sprite '" << filename
                  << "' (" << width << "x" << height << ")" << std::endl;
    } else {
        std::cerr << "SpriteManager: Failed to load sprite '" << filename << "'" << std::endl;
    }

    return texture;
}

SDL_Texture* SpriteManager::getSprite(const std::string& name) const {
    auto it = sprites_.find(name);
    if (it != sprites_.end()) {
        return it->second;
    }
    return nullptr;
}

bool SpriteManager::hasSprite(const std::string& name) const {
    return sprites_.find(name) != sprites_.end();
}

bool SpriteManager::getSpriteDimensions(const std::string& name, int& width, int& height) const {
    auto it = dimensions_.find(name);
    if (it != dimensions_.end()) {
        width = it->second.first;
        height = it->second.second;
        return true;
    }
    return false;
}

void SpriteManager::preloadSprites() {
    // Preload common sprites to avoid loading during gameplay
    std::cout << "SpriteManager: Preloading sprites..." << std::endl;

    // Try to load key sprites (will fail gracefully if not present)
    loadSprite("sinistar_face.png");
    loadSprite("player_ship.png");
    loadSprite("warrior.png");
    loadSprite("worker.png");
    loadSprite("crystal_small.png");
    loadSprite("crystal_medium.png");
    loadSprite("crystal_large.png");

    std::cout << "SpriteManager: Preload complete (" << sprites_.size() << " sprites loaded)" << std::endl;
}

SDL_Texture* SpriteManager::loadTextureFromFile(const std::string& filepath) {
    SDL_Texture* texture = nullptr;
    SDL_Surface* surface = nullptr;

#ifdef HAVE_SDL2_IMAGE
    // Try SDL2_image first (supports PNG, JPG, etc.)
    surface = IMG_Load(filepath.c_str());
    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer_, surface);
        SDL_FreeSurface(surface);

        if (!texture) {
            std::cerr << "SpriteManager: Failed to create texture from " << filepath
                      << " - SDL_Error: " << SDL_GetError() << std::endl;
        }
        return texture;
    }
#endif

    // Fallback: Try SDL_LoadBMP (no external dependency)
    std::string bmpPath = filepath;
    // Replace .png extension with .bmp if present
    size_t pos = bmpPath.find(".png");
    if (pos != std::string::npos) {
        bmpPath.replace(pos, 4, ".bmp");
    }

    surface = SDL_LoadBMP(bmpPath.c_str());
    if (surface) {
        texture = SDL_CreateTextureFromSurface(renderer_, surface);
        SDL_FreeSurface(surface);

        if (!texture) {
            std::cerr << "SpriteManager: Failed to create texture from " << bmpPath
                      << " - SDL_Error: " << SDL_GetError() << std::endl;
        } else {
            std::cout << "SpriteManager: Loaded BMP fallback: " << bmpPath << std::endl;
        }
    }

    return texture;
}
