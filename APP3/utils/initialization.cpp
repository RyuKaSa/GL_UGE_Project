#include "initialization.hpp"
#include <iostream>
#include <GL/gl.h>
#include <GL/glu.h>
#include <glimac/SDLWindowManager.hpp>

namespace utils_init {

glimac::SDLWindowManager initOpenGL(int& window_width, int& window_height) {
    std::cout << "Program started" << std::endl;

    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        throw std::runtime_error("Failed to initialize SDL");
    }

    // Fetch fullscreen modes
    SDL_Rect** modes = SDL_ListModes(NULL, SDL_FULLSCREEN | SDL_OPENGL);
    if (modes == (SDL_Rect**)0) {
        throw std::runtime_error("No fullscreen modes available.");
    }
    if (modes != (SDL_Rect**)-1) {
        window_width = modes[0]->w;
        window_height = modes[0]->h;
    }

    // Create OpenGL window
    SDL_Surface* screen = SDL_SetVideoMode(window_width, window_height, 32, SDL_OPENGL | SDL_FULLSCREEN);
    if (!screen) {
        std::cerr << "Failed to create SDL OpenGL window: " << SDL_GetError() << std::endl;
        throw std::runtime_error("Failed to create SDL OpenGL window");
    }

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // OpenGL setup
    glViewport(0, 0, window_width, window_height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)window_width / (float)window_height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapBuffers();

    // OpenGL version and renderer
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

    return glimac::SDLWindowManager(window_width, window_height, "Boules");
}

} // namespace utils_init
