// utils/Buffer.hpp
#ifndef BUFFER_HPP
#define BUFFER_HPP

#include <glad/glad.h>
#include <glimac/Sphere.hpp>

namespace utils {

/**
 * @brief Creates a Vertex Buffer Object (VBO) and Vertex Array Object (VAO) for a given sphere.
 * 
 * @param vbo Reference to the GLuint where the VBO ID will be stored.
 * @param vao Reference to the GLuint where the VAO ID will be stored.
 * @param sphere The Sphere object containing vertex data.
 */
void createVBOAndVAO(GLuint& vbo, GLuint& vao, const glimac::Sphere& sphere);

/**
 * @brief Creates a full-screen quad and returns its Vertex Array Object (VAO) ID.
 * 
 * @return GLuint The VAO ID of the created full-screen quad.
 */
GLuint createFullScreenQuad();

} // namespace utils

#endif // BUFFER_HPP