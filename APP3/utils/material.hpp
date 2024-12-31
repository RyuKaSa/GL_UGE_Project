#ifndef MATERIAL_HPP
#define MATERIAL_HPP

#include <glm/glm.hpp>
#include "utils/global.hpp"

struct Material
{
    // Diffuse properties
    glm::vec3 Kd;        // Diffuse color
    bool hasDiffuseMap;  // Flag to indicate if diffuse map is used
    GLuint diffuseMapID; // Diffuse texture ID

    // Specular properties
    glm::vec3 Ks;         // Specular color
    float shininess;      // Shininess exponent
    bool hasSpecularMap;  // Flag to indicate if specular map is used
    GLuint specularMapID; // Specular texture ID

    // Normal mapping
    bool hasNormalMap;  // Flag to indicate if normal map is used
    GLuint normalMapID; // Normal texture ID

    float alpha = 1.0f; // Transparency

    bool operator==(const Material& other) const {
        return Kd == other.Kd &&
            hasDiffuseMap == other.hasDiffuseMap &&
            diffuseMapID == other.diffuseMapID &&
            Ks == other.Ks &&
            shininess == other.shininess &&
            hasSpecularMap == other.hasSpecularMap &&
            specularMapID == other.specularMapID &&
            hasNormalMap == other.hasNormalMap &&
            normalMapID == other.normalMapID &&
            alpha == other.alpha;
    }

    // Constructor with default values
    Material()
        : Kd(1.0f, 1.0f, 1.0f),
          hasDiffuseMap(false),
          diffuseMapID(0),
          Ks(0.3f, 0.3f, 0.3f),
          shininess(32.0f),
          hasSpecularMap(false),
          specularMapID(0),
          hasNormalMap(false),
          normalMapID(0),
          alpha(1.0f) {}
};

#endif // MATERIAL_HPP