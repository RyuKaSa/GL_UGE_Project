// utils/Objects.hpp
#ifndef OBJECTS_HPP
#define OBJECTS_HPP

#include <glm/glm.hpp>
#include <vector>

namespace objects {

struct Sphere {
    glm::vec3 position; // Position of the sphere in world space
    float size;         // Scale factor for the sphere
    glm::vec3 color;    // Color of the sphere

    Sphere(const glm::vec3& pos = glm::vec3(0.0f), 
           float s = 1.0f, 
           const glm::vec3& col = glm::vec3(1.0f))
        : position(pos), size(s), color(col) {}
};

// Adds a new sphere to the scene
void addSphere(const glm::vec3& position, float size, const glm::vec3& color);

// Removes a sphere from the scene by its index
void removeSphere(size_t index);

// Moves a sphere to a new position by its index
void moveSphere(size_t index, const glm::vec3& newPosition);

// Retrieves the list of all spheres
const std::vector<Sphere>& getSpheres();

} // namespace objects

#endif // OBJECTS_HPP