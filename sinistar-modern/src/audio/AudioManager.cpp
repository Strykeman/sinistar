/**
 * AudioManager.cpp
 *
 * Implementation of audio system
 */

#include "AudioManager.h"
#include <iostream>

AudioManager& AudioManager::getInstance() {
    static AudioManager instance;
    return instance;
}

AudioManager::AudioManager()
    : initialized_(false)
    , enabled_(true)
    , masterVolume_(0.7f)
{
}

AudioManager::~AudioManager() {
    shutdown();
}

bool AudioManager::initialize() {
#ifdef HAVE_SDL2_MIXER
    // Initialize SDL2_mixer
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "SDL_mixer initialization failed: " << Mix_GetError() << std::endl;
        initialized_ = false;
        return false;
    }

    // Allocate mixing channels
    Mix_AllocateChannels(16);

    // Set initial volume
    Mix_Volume(-1, static_cast<int>(MIX_MAX_VOLUME * masterVolume_));

    // Load sound effects
    loadSounds();

    initialized_ = true;
    std::cout << "AudioManager initialized with SDL2_mixer" << std::endl;
    return true;
#else
    std::cout << "AudioManager: SDL2_mixer not available (audio disabled)" << std::endl;
    initialized_ = false;
    return false;
#endif
}

void AudioManager::shutdown() {
#ifdef HAVE_SDL2_MIXER
    if (!initialized_) return;

    // Free all sounds
    for (auto& pair : sounds_) {
        if (pair.second) {
            Mix_FreeChunk(pair.second);
        }
    }
    sounds_.clear();

    // Close audio
    Mix_CloseAudio();

    initialized_ = false;
    std::cout << "AudioManager shut down" << std::endl;
#endif
}

void AudioManager::playSound(SoundEffect effect, float volume) {
    if (!initialized_ || !enabled_) return;

#ifdef HAVE_SDL2_MIXER
    auto it = sounds_.find(effect);
    if (it != sounds_.end() && it->second) {
        // Set volume for this playback
        int channelVolume = static_cast<int>(MIX_MAX_VOLUME * volume * masterVolume_);
        Mix_VolumeChunk(it->second, channelVolume);

        // Play on first available channel
        Mix_PlayChannel(-1, it->second, 0);
    }
#else
    // Suppress unused parameter warnings
    (void)effect;
    (void)volume;
#endif
}

void AudioManager::setMasterVolume(float volume) {
    masterVolume_ = std::max(0.0f, std::min(1.0f, volume));

#ifdef HAVE_SDL2_MIXER
    if (initialized_) {
        Mix_Volume(-1, static_cast<int>(MIX_MAX_VOLUME * masterVolume_));
    }
#endif
}

void AudioManager::loadSounds() {
#ifdef HAVE_SDL2_MIXER
    // TODO: Load actual sound files when assets are available
    // For now, just create empty placeholders

    // Example:
    // sounds_[SoundEffect::PLAYER_SHOOT] = Mix_LoadWAV("assets/sounds/shoot.wav");
    // if (!sounds_[SoundEffect::PLAYER_SHOOT]) {
    //     std::cerr << "Failed to load shoot sound: " << Mix_GetError() << std::endl;
    // }

    // Generate placeholder sounds (silent for now)
    for (int i = 0; i < static_cast<int>(SoundEffect::COUNT); i++) {
        generatePlaceholderSound(static_cast<SoundEffect>(i));
    }

    std::cout << "AudioManager: Sound placeholders created (no audio files yet)" << std::endl;
#endif
}

void AudioManager::generatePlaceholderSound(SoundEffect effect) {
#ifdef HAVE_SDL2_MIXER
    // Create a very short silent sound as placeholder
    // This prevents crashes when trying to play non-existent sounds
    // Real sounds will be loaded when asset files are available

    // Note: We could generate simple beeps here using SDL2 audio buffers,
    // but for now we'll just set nullptr and check before playing
    sounds_[effect] = nullptr;
#else
    (void)effect;
#endif
}
