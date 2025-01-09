#include "lights.hpp"

namespace utils_light
{

    // A simple static function-scope variable to auto-increment IDs
    // Each call returns a new unique integer ID
    int generateLightID()
    {
        static int s_nextLightID = 0;
        return s_nextLightID++;
    }

    // Adds a new light to the container
    // Returns the newly generated light ID
    int addLight(std::vector<SimplePointLight> &lights,
                 const glm::vec3 &position,
                 const glm::vec3 &color,
                 float intensity)
    {
        SimplePointLight light;
        light.id = generateLightID();
        light.position = position;
        light.color = color;
        light.intensity = intensity;

        lights.push_back(light);
        return light.id; // Return the assigned ID
    }

    // Removes a light from the container by ID
    // Returns true if found and removed, false otherwise
    bool removeLight(std::vector<SimplePointLight> &lights, int lightID)
    {
        for (auto it = lights.begin(); it != lights.end(); ++it)
        {
            if (it->id == lightID)
            {
                lights.erase(it);
                return true;
            }
        }
        return false; // Not found
    }

    // Update a light's position by ID
    void updateLightPosition(std::vector<SimplePointLight> &lights, int lightID, const glm::vec3 &newPosition)
    {
        for (auto &light : lights)
        {
            if (light.id == lightID)
            {
                light.position = newPosition;
                break;
            }
        }
    }

    // Update a light’s color by ID
    void updateLightColor(std::vector<SimplePointLight> &lights, int lightID, const glm::vec3 &newColor)
    {
        for (auto &light : lights)
        {
            if (light.id == lightID)
            {
                light.color = newColor;
                break;
            }
        }
    }

    // Update a light’s intensity by ID
    void updateLightIntensity(std::vector<SimplePointLight> &lights, int lightID, float newIntensity)
    {
        for (auto &light : lights)
        {
            if (light.id == lightID)
            {
                light.intensity = newIntensity;
                break;
            }
        }
    }

    // Simple hash function for pseudo-random generation
    float pseudoRandom(float seed)
    {
        return glm::fract(sin(seed) * 43758.5453f);
    }

    // Update all lights dynamically
    void updateDynamicLights(std::vector<SimplePointLight*> &lights, float currentFrame)
    {
        // Constants for dynamic behavior
        const float HEIGHT_AMPLITUDE = 0.004f;          // Max vertical displacement
        const float COLOR_VARIATION_SPEED = 2.5f;       // Speed of color variation
        const glm::vec3 BASE_COLOR = glm::vec3(1.0f, 0.8f, 0.6f); // Base light color

        // std::cout << "Debug: Number of lights in updateDynamicLights: " << lights.size() << "\n";
        // for (size_t i = 0; i < lights.size(); ++i) {
        //     if (!lights[i]) {
        //         std::cout << "Light[" << i << "] is null!\n";
        //     } else {
        //         std::cout << "Light[" << i << "] Initial Position: (" << lights[i]->position.x << ", "
        //                 << lights[i]->position.y << ", " << lights[i]->position.z << ")\n";
        //     }
        // }


        // Update each light dynamically
        for (size_t i = 0; i < lights.size(); i++) {
            auto light = lights[i];

            // **Dynamic Vertical Movement**
            float verticalOffset = sin(currentFrame * 0.5f + i * 0.5f) * HEIGHT_AMPLITUDE;
            light->position.y += verticalOffset; // Adjust Y-position dynamically

            // **Dynamic Color Variation**
            float colorFactorR = (sin(currentFrame + i * COLOR_VARIATION_SPEED) + 1.0f) / 2.0f;
            float colorFactorG = (sin(currentFrame + i * COLOR_VARIATION_SPEED + 2.0f) + 1.0f) / 2.0f; // Offset phase by 2
            float colorFactorB = (sin(currentFrame + i * COLOR_VARIATION_SPEED + 4.0f) + 1.0f) / 2.0f; // Offset phase by 4

            light->color = glm::vec3(BASE_COLOR.r * colorFactorR,
                                    BASE_COLOR.g * colorFactorG,
                                    BASE_COLOR.b * colorFactorB);

            // **Dynamic Intensity Variation**
            float intensityFactor = (sin(currentFrame + i * COLOR_VARIATION_SPEED) + 1.0f) / 2.0f;
            // Clamp the intensity factor to be between 0.2 and 0.8
            intensityFactor = glm::clamp(intensityFactor, 0.2f, 0.8f);
            light->intensity = intensityFactor;
        }
    }

} // namespace utils_light
