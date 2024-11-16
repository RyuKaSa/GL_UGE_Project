#ifndef POINT_LIGHT_HPP
#define POINT_LIGHT_HPP

#include <glm/glm.hpp>

namespace Point_Light {

    struct PointLight {
        glm::vec3 position;  // Position of the point light
        glm::vec3 color;     // Color of the point light (RGB)
        float intensity;     // Intensity of the light (scalar multiplier)
        glm::vec3 direction; // Direction of the light

        // Shadow map properties
        unsigned int shadowMapFBO; // Framebuffer object for shadow mapping
        unsigned int shadowMap;    // Texture ID for the shadow map

        // Optional properties if needed
        float constantAttenuation = 1.0f;
        float linearAttenuation = 0.09f;
        float quadraticAttenuation = 0.032f;

        // Constructor
        PointLight() 
            : position(glm::vec3(0.0f)), color(glm::vec3(1.0f)), intensity(1.0f), direction(glm::vec3(0.0f, -1.0f, 0.0f)),
              shadowMapFBO(0), shadowMap(0) {}
    };

} // namespace Point_Light

#endif // POINT_LIGHT_HPP