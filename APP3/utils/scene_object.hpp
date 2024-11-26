#ifndef SCENE_OBJECT_HPP
#define SCENE_OBJECT_HPP

#include "utilities.hpp"
#include "global.hpp"
#include <vector>
#include <map>
#include <string>

namespace utils_scene {

enum class ObjectType {
    Cube,
    Sphere,
    Model
};

struct SceneObject {
    std::string name;
    ObjectType type;
    glm::vec3 position;
    glm::vec3 initialPosition;
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
    bool isStatic;
};

extern std::vector<SceneObject> sceneObjects;

void addCube(const std::string& name, const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color, bool useTexture, GLuint textureID = 0, GLuint normalMapID = 0, const glm::vec3& rotationAxis = glm::vec3(0.0f), float rotationAngle = 0.0f, GLuint vaoID = 0, GLsizei indexCount = 0, bool isStatic = false);

void addSphere(const std::string& name, const glm::vec3& position, float radius, const glm::vec3& color, bool useTexture, GLuint textureID = 0, GLuint normalMapID = 0, GLuint vaoID = 0, GLsizei vertexCount = 0, bool isStatic = false);

void createCompositeCube(const std::string& name, const glm::vec3& origin, const glm::vec3& size, GLuint textureID, GLuint normalMapID, GLuint vaoID, GLsizei indexCount, bool isStatic);

void addModel(const std::string& name, const glm::vec3& position, const glm::vec3& scale, bool useTexture, GLuint textureID, GLuint normalMapID, GLuint vaoID, GLsizei indexCount, const AABB& boundingBox, const glm::vec3& rotationAxis = glm::vec3(0.0f), float rotationAngle = 0.0f, bool isStatic = false);

} // namespace utils_scene

#endif // SCENE_OBJECT_HPP