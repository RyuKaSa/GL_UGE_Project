// material_manager.hpp
#ifndef MATERIAL_MANAGER_HPP
#define MATERIAL_MANAGER_HPP

#include "material.hpp"
#include <vector>
#include <string>
#include <iostream> // For debugging

class MaterialManager {
public:
    // Adds a new material and returns its index
    // If the material already exists, returns the existing index
    int addOrGetMaterial(const Material& material) {
        int index = findMaterial(material);
        if(index != -1) {
            std::cout << "Material already exists. Returning existing index: " << index << std::endl;
            return index;
        }
        materials.push_back(material);
        std::cout << "Added new material. New index: " << static_cast<int>(materials.size() - 1) << std::endl;
        return static_cast<int>(materials.size() - 1);
    }

    // Retrieves a material by index
    const Material& getMaterial(int index) const {
        if (index >= 0 && index < static_cast<int>(materials.size())) {
            return materials[index];
        }
        // Handle invalid index appropriately (e.g., return a default material or throw an error)
        // Here, we'll return the last material as a fallback
        std::cerr << "Invalid material index: " << index << ". Returning last material." << std::endl;
        return materials.back();
    }

    // Singleton pattern for global access
    static MaterialManager& getInstance() {
        static MaterialManager instance;
        return instance;
    }

    // Finds a material and returns its index, or -1 if not found
    int findMaterial(const Material& material) const {
        for (size_t i = 0; i < materials.size(); ++i) {
            if (materials[i] == material) { // Uses the overloaded operator==
                return static_cast<int>(i); // Return index if found
            }
        }
        return -1; // Not found
    }

    std::vector<Material> materials;

private:
    // Private constructor for Singleton
    MaterialManager() {}
    // Delete copy constructor and assignment operator
    MaterialManager(const MaterialManager&) = delete;
    MaterialManager& operator=(const MaterialManager&) = delete;
};

#endif // MATERIAL_MANAGER_HPP