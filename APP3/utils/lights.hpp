#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#include <glm/glm.hpp>
#include <vector>
#include <iostream>

#include "pointer.hpp"

namespace utils_light {

    // ---------------------------
    // Struct for a simple point light
    // ---------------------------
    struct SimplePointLight {
        int id;              // Unique identifier (e.g., auto-incrementing)
        glm::vec3 position;  // Position in world space
        glm::vec3 color;     // Color (could be considered 'diffuse' color)
        float intensity;     // Brightness multiplier
    };

    // ---------------------------
    // Light management functions
    // ---------------------------
    
    // Generates a unique ID for a light
    int generateLightID();

    // Adds a new light to the container, returns assigned ID
    int addLight(std::vector<SimplePointLight>& lights, 
                 const glm::vec3& position, 
                 const glm::vec3& color, 
                 float intensity);

    // Removes an existing light by ID, returns true on success
    bool removeLight(std::vector<SimplePointLight>& lights, int lightID);

    // Moves a light’s position
    void updateLightPosition(std::vector<SimplePointLight>& lights, int lightID, const glm::vec3& newPosition);

    // Changes a light’s color
    void updateLightColor(std::vector<SimplePointLight>& lights, int lightID, const glm::vec3& newColor);

    // Changes a light’s intensity
    void updateLightIntensity(std::vector<SimplePointLight>& lights, int lightID, float newIntensity);

    // Generates a pseudo-random value based on a seed
    float pseudoRandom(float seed);

    // void update simple light pso and colors
    void updateDynamicLights(std::vector<SimplePointLight*> &lights, float currentFrame);} // namespace utils_light

#endif // LIGHTS_HPP
