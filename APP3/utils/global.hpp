#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

// Window dimensions
extern int window_width;
extern int window_height;

// Stone floor coordinates
extern int numCubesX;
extern int numCubesZ;
extern float spacingX;
extern float spacingZ;

extern const GLuint SHADOW_WIDTH;
extern const GLuint SHADOW_HEIGHT;

extern float cameraRadius; // Radius of the camera sphere for collision detection
extern float cameraHeight; // Height of the camera cylinder

extern bool isRockingChairPaused;
extern double rockingChairPausedTime;
extern double rockingChairStartTime;

extern float sensitivity;
extern float currentFrame;

extern int MAX_ADDITIONAL_LIGHTS;

extern bool wireframeMode;

// Structure for 3D vertices with position, normal, and texture coordinates
struct Vertex3D {
    glm::vec3 position;  // Vertex position
    glm::vec3 normal;    // Vertex normal
    glm::vec2 texCoords; // Texture coordinates
    glm::vec3 tangent;
    glm::vec3 bitangent;

    Vertex3D(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& uv)
        : position(pos), normal(norm), texCoords(uv), tangent(0.0f), bitangent(0.0f) {}
};

struct SphereVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

#endif // GLOBAL_HPP