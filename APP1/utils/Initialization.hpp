// utils/Initialization.hpp
#ifndef INITIALIZATION_HPP
#define INITIALIZATION_HPP

#include <glad/glad.h>
#include <SDL2/SDL.h>
#include <string>
#include <glimac/SDLWindowManager.hpp>

namespace utils {

// Initializes SDL, creates a window, and initializes GLAD
// Returns true on success, false on failure
bool initializeSDL_GL(int width, int height, const std::string& title, glimac::SDLWindowManager& windowManager);

} // namespace utils

#endif // INITIALIZATION_HPP