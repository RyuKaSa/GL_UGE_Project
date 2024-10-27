#pragma once

#include <cstdint>
#include <SDL2/SDL.h>
#include "glm.hpp"

namespace glimac {

class SDLWindowManager {
public:
    SDLWindowManager(uint32_t width, uint32_t height, const char* title);

    ~SDLWindowManager();

    bool pollEvent(SDL_Event& e);

    bool isKeyPressed(SDL_Keycode key) const;

    // Button can be SDL_BUTTON_LEFT, SDL_BUTTON_RIGHT, or SDL_BUTTON_MIDDLE
    bool isMouseButtonPressed(uint32_t button) const;

    glm::ivec2 getMousePosition() const;

    void swapBuffers();

    // Returns the time in seconds
    float getTime() const;

private:
    SDL_Window* m_window;
    SDL_GLContext m_context;
};

}