// utils/Objects.cpp
#include "Objects.hpp"
#include <iostream>

namespace objects {

// Internal list to store all spheres
static std::vector<Sphere> spheres;

// Adds a new sphere to the scene
void addSphere(const glm::vec3& position, float size, const glm::vec3& color) {
    spheres.emplace_back(Sphere(position, size, color));
    std::cout << "Added sphere at position (" 
              << position.x << ", " << position.y << ", " << position.z 
              << ") with size " << size << " and color (" 
              << color.r << ", " << color.g << ", " << color.b << ")." 
              << std::endl;
}

// Removes a sphere from the scene by its index
void removeSphere(size_t index) {
    if (index < spheres.size()) {
        std::cout << "Removing sphere at index " << index << "." << std::endl;
        spheres.erase(spheres.begin() + index);
    } else {
        std::cerr << "removeSphere: Index " << index << " is out of range! No sphere removed." << std::endl;
    }
}

// Moves a sphere to a new position by its index
void moveSphere(size_t index, const glm::vec3& newPosition) {
    if (index < spheres.size()) {
        std::cout << "Moving sphere at index " << index 
                  << " to new position (" 
                  << newPosition.x << ", " << newPosition.y << ", " << newPosition.z 
                  << ")." << std::endl;
        spheres[index].position = newPosition;
    } else {
        std::cerr << "moveSphere: Index " << index << " is out of range! No sphere moved." << std::endl;
    }
}

// Retrieves the list of all spheres
const std::vector<Sphere>& getSpheres() {
    return spheres;
}

} // namespace objects