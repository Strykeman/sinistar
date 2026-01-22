/**
 * RenderSystem.cpp
 *
 * Implementation of rendering system
 */

#include "RenderSystem.h"
#include "../entities/GameObject.h"
#include <algorithm>
#include <iostream>

RenderSystem::RenderSystem(SDL_Renderer* renderer)
    : renderer_(renderer)
{
    std::cout << "RenderSystem initialized" << std::endl;
}

RenderSystem::~RenderSystem() {
    clear();
    std::cout << "RenderSystem destroyed" << std::endl;
}

void RenderSystem::render() {
    // Render all registered objects
    for (GameObject* object : objects_) {
        if (object && object->isActive()) {
            object->render();
        }
    }
}

void RenderSystem::registerObject(GameObject* object) {
    if (object) {
        objects_.push_back(object);
    }
}

void RenderSystem::unregisterObject(GameObject* object) {
    objects_.erase(
        std::remove(objects_.begin(), objects_.end(), object),
        objects_.end()
    );
}

void RenderSystem::clear() {
    objects_.clear();
}
