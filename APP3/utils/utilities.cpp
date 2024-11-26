#include "utilities.hpp"
#include <cstdlib>

float randomFloat() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

glm::vec3 randomColor() {
    return glm::vec3(0.6f);
}

AABB computeAABB(const std::vector<float>& vertices) {
    if (vertices.empty()) {
        return AABB(glm::vec3(0.0f), glm::vec3(0.0f));
    }
    
    glm::vec3 minVertex(vertices[0], vertices[1], vertices[2]);
    glm::vec3 maxVertex = minVertex;
    
    for (size_t i = 3; i < vertices.size(); i += 3) {
        glm::vec3 vertex(vertices[i], vertices[i + 1], vertices[i + 2]);
        minVertex = glm::min(minVertex, vertex);
        maxVertex = glm::max(maxVertex, vertex);
    }
    
    return AABB(minVertex, maxVertex);
}