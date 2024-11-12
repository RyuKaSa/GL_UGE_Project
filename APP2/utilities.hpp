#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <glm/glm.hpp>
#include <vector>

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
    
    AABB() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {}
    AABB(const glm::vec3& min_, const glm::vec3& max_) : min(min_), max(max_) {}
};

float randomFloat();

glm::vec3 randomColor();

// Function to compute AABB from vertices
AABB computeAABB(const std::vector<float>& vertices);

#endif // UTILITIES_HPP