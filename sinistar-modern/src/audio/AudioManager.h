/**
 * AudioManager.h
 *
 * Audio management system for sound effects and music
 * Uses SDL2_mixer for cross-platform audio
 */

#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <SDL.h>
#ifdef HAVE_SDL2_MIXER
#include <SDL_mixer.h>
#endif
#include <map>
#include <string>

/**
 * Sound effect types
 */
enum class SoundEffect {
    PLAYER_SHOOT,
    ENEMY_EXPLODE,
    PLAYER_HIT,
    ENEMY_HIT,
    SHIELD_DOWN,
    PICKUP_CRYSTAL,
    PLAYER_DIE,
    SINIBOMB_EXPLODE,
    COUNT
};

/**
 * AudioManager - handles all audio playback
 * Singleton pattern for global access
 */
class AudioManager {
public:
    /**
     * Get singleton instance
     */
    static AudioManager& getInstance();

    /**
     * Initialize audio system
     * @return true if successful
     */
    bool initialize();

    /**
     * Shutdown audio system
     */
    void shutdown();

    /**
     * Play a sound effect
     * @param effect Sound effect to play
     * @param volume Volume (0.0 to 1.0)
     */
    void playSound(SoundEffect effect, float volume = 1.0f);

    /**
     * Set master volume
     * @param volume Volume (0.0 to 1.0)
     */
    void setMasterVolume(float volume);

    /**
     * Get master volume
     */
    float getMasterVolume() const { return masterVolume_; }

    /**
     * Enable/disable sound
     */
    void setEnabled(bool enabled) { enabled_ = enabled; }

    /**
     * Check if sound is enabled
     */
    bool isEnabled() const { return enabled_; }

    /**
     * Check if audio system is initialized
     */
    bool isInitialized() const { return initialized_; }

private:
    AudioManager();
    ~AudioManager();

    // Prevent copying
    AudioManager(const AudioManager&) = delete;
    AudioManager& operator=(const AudioManager&) = delete;

    /**
     * Load all sound effects
     */
    void loadSounds();

    /**
     * Generate placeholder sound for missing audio files
     */
    void generatePlaceholderSound(SoundEffect effect);

    bool initialized_;
    bool enabled_;
    float masterVolume_;

#ifdef HAVE_SDL2_MIXER
    std::map<SoundEffect, Mix_Chunk*> sounds_;
#endif
};

#endif // AUDIOMANAGER_H
