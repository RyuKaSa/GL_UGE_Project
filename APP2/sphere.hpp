#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "global.hpp"
#include <vector>
#include <glimac/Sphere.hpp>

size_t createSphereVertices(std::vector<SphereVertex>& sphereVertices, glimac::Sphere& sphere);
void computeSphereTangents(std::vector<SphereVertex>& vertices);
void setupSphereBuffers(const std::vector<SphereVertex>& sphereVertices, GLuint& sphereVBO, GLuint& sphereVAO);

#endif // SPHERE_HPP