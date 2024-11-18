// utils/Buffer.hpp
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <glad/glad.h>
#include <glimac/Sphere.hpp>

namespace utils {

void createVBOAndVAO(GLuint& vbo, GLuint& vao, const glimac::Sphere& sphere);

GLuint createFullScreenQuad();

} // namespace utils

#endif // BUFFER_HPP