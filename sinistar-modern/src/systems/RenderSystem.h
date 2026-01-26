/**
 * RenderSystem.h
 *
 * Rendering system - handles all graphics rendering
 * Replicates the display system from DRAWOBJ.SRC, IMAGE.SRC, etc.
 */

#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <SDL.h>
#include <vector>

class GameObject;

class RenderSystem {
public:
    /**
     * Constructor
     * @param renderer SDL renderer to use
     */
    explicit RenderSystem(SDL_Renderer* renderer);

    /**
     * Destructor
     */
    ~RenderSystem();

    /**
     * Render all registered objects
     */
    void render();

    /**
     * Register an object for rendering
     * @param object GameObject to render
     */
    void registerObject(GameObject* object);

    /**
     * Unregister an object
     * @param object GameObject to unregister
     */
    void unregisterObject(GameObject* object);

    /**
     * Clear all registered objects
     */
    void clear();

private:
    SDL_Renderer* renderer_;
    std::vector<GameObject*> objects_;
};

#endif // RENDERSYSTEM_H
