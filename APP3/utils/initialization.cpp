#include "initialization.hpp"
#include <iostream>
#include <glad/glad.h>
#include <glimac/SDLWindowManager.hpp>

namespace utils_init {

glimac::SDLWindowManager initOpenGL(int window_width, int window_height) {
    std::cout << "Program started" << std::endl;

    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        throw std::runtime_error("Failed to initialize SDL");
    }
    else
    {
        std::cout << "SDL video subsystem initialized" << std::endl;
    }

    // Set OpenGL context version and profile
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) != 0)
        std::cerr << "Failed to set SDL_GL_CONTEXT_MAJOR_VERSION: " << SDL_GetError() << std::endl;
    else
        std::cout << "SDL_GL_CONTEXT_MAJOR_VERSION set to 3" << std::endl;

    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0)
        std::cerr << "Failed to set SDL_GL_CONTEXT_MINOR_VERSION: " << SDL_GetError() << std::endl;
    else
        std::cout << "SDL_GL_CONTEXT_MINOR_VERSION set to 3" << std::endl;

    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0)
        std::cerr << "Failed to set SDL_GL_CONTEXT_PROFILE_MASK: " << SDL_GetError() << std::endl;
    else
        std::cout << "SDL_GL_CONTEXT_PROFILE_MASK set to CORE" << std::endl;

    #ifdef __APPLE__
        if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG) != 0)
            std::cerr << "Failed to set SDL_GL_CONTEXT_FLAGS: " << SDL_GetError() << std::endl;
        else
            std::cout << "SDL_GL_CONTEXT_FLAGS set to forward compatible" << std::endl;
    #endif

    // Initialize SDLWindowManager
    glimac::SDLWindowManager windowManager(window_width, window_height, "Boules");
    std::cout << "SDLWindowManager initialized with size (" << window_width << ", " << window_height << ")" << std::endl;

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        throw std::runtime_error("Failed to initialize GLAD");
    }
    else
    {
        std::cout << "GLAD initialized" << std::endl;
    }

    // Output OpenGL version and renderer
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    if (renderer)
        std::cout << "Renderer: " << renderer << std::endl;
    else
        std::cout << "Renderer is NULL" << std::endl;

    if (version)
        std::cout << "OpenGL version supported: " << version << std::endl;
    else
        std::cout << "OpenGL version is NULL" << std::endl;

    // Check available OpenGL extensions
    const GLubyte *extensions = glGetString(GL_EXTENSIONS);
    if (extensions)
    {
        std::cout << "Available extensions:\n" << extensions << std::endl;
    }
    else
    {
        std::cerr << "No OpenGL extensions available." << std::endl;
    }

    // Check if glGenVertexArrays is available
    if (glGenVertexArrays == nullptr)
    {
        std::cerr << "Error: glGenVertexArrays is NULL" << std::endl;
        throw std::runtime_error("glGenVertexArrays is NULL");
    }
    else
    {
        std::cout << "glGenVertexArrays is available" << std::endl;
    }

    std::cout << "OpenGL context and window manager initialized successfully." << std::endl;

    return windowManager;
}

} // namespace utils_init