#ifndef INIT_HPP
#define INIT_HPP

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <glimac/Sphere.hpp>
#include <iostream>


namespace utils {

    void createVBOAndVAO(GLuint& vbo, GLuint& vao, const glimac::Sphere& sphere);

} // namespace utils

#endif // INIT_HPP