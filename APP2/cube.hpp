#ifndef CUBE_HPP
#define CUBE_HPP

#include "global.hpp"
#include <vector>

void createCube(std::vector<Vertex3D>& vertices, std::vector<GLuint>& indices);
void computeTangents(std::vector<Vertex3D>& vertices, const std::vector<GLuint>& indices);
void setupCubeBuffers(const std::vector<Vertex3D>& vertices, const std::vector<GLuint>& indices, GLuint& cubeVBO, GLuint& cubeEBO, GLuint& cubeVAO);

#endif // CUBE_HPP