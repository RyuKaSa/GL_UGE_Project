#include "utilities.hpp"
#include <cstdlib>

float randomFloat() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

glm::vec3 randomColor() {
    return glm::vec3(0.6f);
}