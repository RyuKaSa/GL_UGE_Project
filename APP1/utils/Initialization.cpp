// utils/Initialization.cpp
#include "Initialization.hpp"
#include <iostream>

namespace utils {

bool initializeSDL_GL(int width, int height, const std::string& title, glimac::SDLWindowManager& windowManager) {
    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return false;
    }

    // Set OpenGL context version and profile
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) != 0) {
        std::cerr << "Failed to set SDL_GL_CONTEXT_MAJOR_VERSION: " << SDL_GetError() << std::endl;
        return false;
    }
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0) {
        std::cerr << "Failed to set SDL_GL_CONTEXT_MINOR_VERSION: " << SDL_GetError() << std::endl;
        return false;
    }
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0) {
        std::cerr << "Failed to set SDL_GL_CONTEXT_PROFILE_MASK: " << SDL_GetError() << std::endl;
        return false;
    }

#ifdef __APPLE__
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG) != 0) {
        std::cerr << "Failed to set SDL_GL_CONTEXT_FLAGS: " << SDL_GetError() << std::endl;
        return false;
    }
#endif

    // Create the window manager
    std::cout << "SDLWindowManager initialized" << std::endl;

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }
    std::cout << "GLAD initialized" << std::endl;

    // Output OpenGL version and renderer
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    if (renderer)
        std::cout << "Renderer: " << renderer << std::endl;
    else
        std::cout << "Renderer is NULL" << std::endl;

    if (version)
        std::cout << "OpenGL version supported: " << version << std::endl;
    else
        std::cout << "OpenGL version is NULL" << std::endl;

    return true;
}

} // namespace utils