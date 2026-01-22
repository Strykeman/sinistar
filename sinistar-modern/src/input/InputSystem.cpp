/**
 * InputSystem.cpp
 *
 * Implementation of input system
 */

#include "InputSystem.h"
#include <iostream>
#include <cstring>

InputSystem& InputSystem::getInstance() {
    static InputSystem instance;
    return instance;
}

InputSystem::InputSystem()
    : keyboardState_(nullptr)
    , mousePosition_(0, 0)
    , mouseButtonState_(0)
    , gamepad_(nullptr)
{
    // Initialize button states
    std::memset(buttonDown_, 0, sizeof(buttonDown_));
    std::memset(buttonPressed_, 0, sizeof(buttonPressed_));
    std::memset(buttonReleased_, 0, sizeof(buttonReleased_));
    std::memset(prevButtonDown_, 0, sizeof(prevButtonDown_));

    // Get keyboard state
    keyboardState_ = SDL_GetKeyboardState(nullptr);

    // Try to open first gamepad
    openGamepad();

    std::cout << "InputSystem initialized" << std::endl;
    if (hasGamepad()) {
        std::cout << "Gamepad detected: " << SDL_GameControllerName(gamepad_) << std::endl;
    }
}

InputSystem::~InputSystem() {
    closeGamepad();
    std::cout << "InputSystem destroyed" << std::endl;
}

void InputSystem::processEvent(const SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN:
            if (!event.key.repeat) {
                keyPressed_[event.key.keysym.sym] = true;
            }
            break;

        case SDL_KEYUP:
            keyReleased_[event.key.keysym.sym] = true;
            break;

        case SDL_MOUSEMOTION:
            mousePosition_.x = static_cast<float>(event.motion.x);
            mousePosition_.y = static_cast<float>(event.motion.y);
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
            mouseButtonState_ = SDL_GetMouseState(nullptr, nullptr);
            break;

        case SDL_CONTROLLERDEVICEADDED:
            if (!hasGamepad()) {
                openGamepad();
            }
            break;

        case SDL_CONTROLLERDEVICEREMOVED:
            if (hasGamepad() && event.cdevice.which == SDL_JoystickInstanceID(
                SDL_GameControllerGetJoystick(gamepad_))) {
                closeGamepad();
            }
            break;

        default:
            break;
    }
}

void InputSystem::update() {
    // Update keyboard state
    keyboardState_ = SDL_GetKeyboardState(nullptr);

    // Save previous button states
    std::memcpy(prevButtonDown_, buttonDown_, sizeof(buttonDown_));

    // Update button states
    for (int i = 0; i < static_cast<int>(InputButton::COUNT); i++) {
        InputButton button = static_cast<InputButton>(i);
        buttonDown_[i] = getButtonState(button);
        buttonPressed_[i] = buttonDown_[i] && !prevButtonDown_[i];
        buttonReleased_[i] = !buttonDown_[i] && prevButtonDown_[i];
    }

    // Clear per-frame key states
    keyPressed_.clear();
    keyReleased_.clear();
}

bool InputSystem::isButtonDown(InputButton button) const {
    int index = static_cast<int>(button);
    return buttonDown_[index];
}

bool InputSystem::isButtonPressed(InputButton button) const {
    int index = static_cast<int>(button);
    return buttonPressed_[index];
}

bool InputSystem::isButtonReleased(InputButton button) const {
    int index = static_cast<int>(button);
    return buttonReleased_[index];
}

bool InputSystem::isKeyDown(SDL_Keycode key) const {
    SDL_Scancode scancode = SDL_GetScancodeFromKey(key);
    return keyboardState_[scancode];
}

bool InputSystem::isKeyPressed(SDL_Keycode key) const {
    auto it = keyPressed_.find(key);
    return it != keyPressed_.end() && it->second;
}

Vector2 InputSystem::getMovementAxis() const {
    Vector2 axis(0, 0);

    // Keyboard - arrow keys or WASD
    if (isKeyDown(SDLK_UP) || isKeyDown(SDLK_w)) {
        axis.y -= 1.0f;
    }
    if (isKeyDown(SDLK_DOWN) || isKeyDown(SDLK_s)) {
        axis.y += 1.0f;
    }
    if (isKeyDown(SDLK_LEFT) || isKeyDown(SDLK_a)) {
        axis.x -= 1.0f;
    }
    if (isKeyDown(SDLK_RIGHT) || isKeyDown(SDLK_d)) {
        axis.x += 1.0f;
    }

    // Gamepad left stick
    if (hasGamepad()) {
        float padX = SDL_GameControllerGetAxis(gamepad_, SDL_CONTROLLER_AXIS_LEFTX) / 32767.0f;
        float padY = SDL_GameControllerGetAxis(gamepad_, SDL_CONTROLLER_AXIS_LEFTY) / 32767.0f;

        // Apply deadzone
        const float DEADZONE = 0.15f;
        if (std::abs(padX) > DEADZONE) {
            axis.x += padX;
        }
        if (std::abs(padY) > DEADZONE) {
            axis.y += padY;
        }
    }

    // Clamp to -1 to 1
    if (axis.lengthSquared() > 1.0f) {
        axis.normalize();
    }

    return axis;
}

float InputSystem::getRotationAxis() const {
    float rotation = 0.0f;

    // Keyboard
    if (isButtonDown(InputButton::ROTATE_LEFT)) {
        rotation -= 1.0f;
    }
    if (isButtonDown(InputButton::ROTATE_RIGHT)) {
        rotation += 1.0f;
    }

    // Gamepad right stick or triggers
    if (hasGamepad()) {
        float padX = SDL_GameControllerGetAxis(gamepad_, SDL_CONTROLLER_AXIS_RIGHTX) / 32767.0f;

        const float DEADZONE = 0.15f;
        if (std::abs(padX) > DEADZONE) {
            rotation += padX;
        }
    }

    return Math::clamp(rotation, -1.0f, 1.0f);
}

bool InputSystem::isMouseButtonDown(int button) const {
    return (mouseButtonState_ & SDL_BUTTON(button)) != 0;
}

void InputSystem::openGamepad() {
    if (hasGamepad()) return;

    // Open first available gamepad
    for (int i = 0; i < SDL_NumJoysticks(); i++) {
        if (SDL_IsGameController(i)) {
            gamepad_ = SDL_GameControllerOpen(i);
            if (gamepad_) {
                std::cout << "Gamepad opened: " << SDL_GameControllerName(gamepad_) << std::endl;
                break;
            }
        }
    }
}

void InputSystem::closeGamepad() {
    if (hasGamepad()) {
        SDL_GameControllerClose(gamepad_);
        gamepad_ = nullptr;
        std::cout << "Gamepad closed" << std::endl;
    }
}

bool InputSystem::getButtonState(InputButton button) const {
    switch (button) {
        case InputButton::THRUST:
            return isKeyDown(SDLK_UP) || isKeyDown(SDLK_w) ||
                   (hasGamepad() && SDL_GameControllerGetButton(gamepad_, SDL_CONTROLLER_BUTTON_A));

        case InputButton::ROTATE_LEFT:
            return isKeyDown(SDLK_LEFT) || isKeyDown(SDLK_a) ||
                   (hasGamepad() && SDL_GameControllerGetButton(gamepad_, SDL_CONTROLLER_BUTTON_DPAD_LEFT));

        case InputButton::ROTATE_RIGHT:
            return isKeyDown(SDLK_RIGHT) || isKeyDown(SDLK_d) ||
                   (hasGamepad() && SDL_GameControllerGetButton(gamepad_, SDL_CONTROLLER_BUTTON_DPAD_RIGHT));

        case InputButton::FIRE:
            return isKeyDown(SDLK_SPACE) || isKeyDown(SDLK_LCTRL) ||
                   (hasGamepad() && (SDL_GameControllerGetButton(gamepad_, SDL_CONTROLLER_BUTTON_X) ||
                                     SDL_GameControllerGetButton(gamepad_, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)));

        case InputButton::PAUSE:
            return isKeyDown(SDLK_p) || isKeyDown(SDLK_PAUSE) ||
                   (hasGamepad() && SDL_GameControllerGetButton(gamepad_, SDL_CONTROLLER_BUTTON_START));

        case InputButton::QUIT:
            return isKeyDown(SDLK_ESCAPE) ||
                   (hasGamepad() && SDL_GameControllerGetButton(gamepad_, SDL_CONTROLLER_BUTTON_BACK));

        default:
            return false;
    }
}
