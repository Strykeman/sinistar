/**
 * InputSystem.h
 *
 * Input handling system for keyboard and gamepad
 * Manages input state and provides convenient access
 */

#ifndef INPUTSYSTEM_H
#define INPUTSYSTEM_H

#include <SDL2/SDL.h>
#include <map>
#include "../core/Math.h"

/**
 * InputButton - abstract button that can be keyboard or gamepad
 */
enum class InputButton {
    // Movement
    THRUST,
    ROTATE_LEFT,
    ROTATE_RIGHT,

    // Weapons
    FIRE,

    // System
    PAUSE,
    QUIT,

    COUNT
};

/**
 * InputSystem - handles all input (keyboard, gamepad, mouse)
 * Singleton pattern for global access
 */
class InputSystem {
public:
    /**
     * Get singleton instance
     */
    static InputSystem& getInstance();

    /**
     * Update input state (call once per frame before game logic)
     * @param event SDL event to process
     */
    void processEvent(const SDL_Event& event);

    /**
     * Update state at end of frame
     */
    void update();

    /**
     * Check if button is currently pressed
     */
    bool isButtonDown(InputButton button) const;

    /**
     * Check if button was just pressed this frame
     */
    bool isButtonPressed(InputButton button) const;

    /**
     * Check if button was just released this frame
     */
    bool isButtonReleased(InputButton button) const;

    /**
     * Get keyboard key state
     */
    bool isKeyDown(SDL_Keycode key) const;

    /**
     * Get keyboard key pressed (this frame only)
     */
    bool isKeyPressed(SDL_Keycode key) const;

    /**
     * Get movement vector (-1 to 1 for each axis)
     * Based on arrow keys or WASD or gamepad
     */
    Vector2 getMovementAxis() const;

    /**
     * Get rotation input (-1 = left, 0 = none, 1 = right)
     */
    float getRotationAxis() const;

    /**
     * Check if any gamepad is connected
     */
    bool hasGamepad() const { return gamepad_ != nullptr; }

    /**
     * Get mouse position
     */
    Vector2 getMousePosition() const { return mousePosition_; }

    /**
     * Check if mouse button is down
     */
    bool isMouseButtonDown(int button) const;

private:
    InputSystem();
    ~InputSystem();

    // Prevent copying
    InputSystem(const InputSystem&) = delete;
    InputSystem& operator=(const InputSystem&) = delete;

    /**
     * Open first available gamepad
     */
    void openGamepad();

    /**
     * Close current gamepad
     */
    void closeGamepad();

    /**
     * Map button to keyboard/gamepad state
     */
    bool getButtonState(InputButton button) const;

    // Keyboard state
    const Uint8* keyboardState_;
    std::map<SDL_Keycode, bool> keyPressed_;
    std::map<SDL_Keycode, bool> keyReleased_;

    // Mouse state
    Vector2 mousePosition_;
    Uint32 mouseButtonState_;

    // Gamepad state
    SDL_GameController* gamepad_;

    // Button state tracking
    bool buttonDown_[static_cast<int>(InputButton::COUNT)];
    bool buttonPressed_[static_cast<int>(InputButton::COUNT)];
    bool buttonReleased_[static_cast<int>(InputButton::COUNT)];
    bool prevButtonDown_[static_cast<int>(InputButton::COUNT)];
};

#endif // INPUTSYSTEM_H
