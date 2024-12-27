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

} // namespace utils_light
