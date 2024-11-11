#ifndef SCENE_OBJECT_HPP
#define SCENE_OBJECT_HPP

#include "global.hpp"
#include <vector>
#include <map>

enum class ObjectType {
    Cube,
    Sphere
};

struct SceneObject {
    ObjectType type;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 color;
    bool useTexture;
    GLuint textureID;
    GLuint normalMapID;
    glm::vec3 rotationAxis;
    float rotationAngle;
    AABB boundingBox;
    GLuint vaoID;
    GLsizei indexCount;
};

extern std::vector<SceneObject> sceneObjects;

void addCube(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color, bool useTexture, GLuint textureID = 0, GLuint normalMapID = 0, const glm::vec3& rotationAxis = glm::vec3(0.0f), float rotationAngle = 0.0f, GLuint vaoID = 0, GLsizei indexCount = 0);

void addSphere(const glm::vec3& position, float radius, const glm::vec3& color, bool useTexture, GLuint textureID = 0, GLuint normalMapID = 0, GLuint vaoID = 0, GLsizei vertexCount = 0);

void createCompositeCube(const glm::vec3& origin, const glm::vec3& size, GLuint textureID, GLuint normalMapID, GLuint vaoID, GLsizei indexCount);

#endif // SCENE_OBJECT_HPP