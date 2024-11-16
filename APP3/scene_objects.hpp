#ifndef SCENE_OBJECT_HPP
#define SCENE_OBJECT_HPP

#include "global.hpp"
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>

// Enums and Structs
enum class ObjectType {
    Cube,
    Sphere,
    Model
};

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
    AABB() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {}
    AABB(const glm::vec3& min_, const glm::vec3& max_) : min(min_), max(max_) {}
};

AABB computeAABB(const std::vector<float>& vertices);

// Function declarations
namespace SceneObjectManager {

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

    extern std::vector<SceneObject> allObjects;
    extern std::vector<SceneObject*> dynamicObjects;
    extern std::vector<SceneObject*> visibleObjects;

    void addCube(const std::string& name, const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color, bool useTexture, GLuint textureID = 0, GLuint normalMapID = 0, const glm::vec3& rotationAxis = glm::vec3(0.0f), float rotationAngle = 0.0f, GLuint vaoID = 0, GLsizei indexCount = 0, bool isStatic = false);
    void addSphere(const std::string& name, const glm::vec3& position, float radius, const glm::vec3& color, bool useTexture, GLuint textureID = 0, GLuint normalMapID = 0, GLuint vaoID = 0, GLsizei vertexCount = 0, bool isStatic = false);
    void createCompositeCube(const std::string& name, const glm::vec3& origin, const glm::vec3& size, GLuint textureID, GLuint normalMapID, GLuint vaoID, GLsizei indexCount, bool isStatic);
    void addModel(const std::string& name, const glm::vec3& position, const glm::vec3& scale, bool useTexture, GLuint textureID, GLuint normalMapID, GLuint vaoID, GLsizei indexCount, const AABB& boundingBox, const glm::vec3& rotationAxis = glm::vec3(0.0f), float rotationAngle = 0.0f, bool isStatic = false);

    bool checkCollision(const glm::vec3& cylinderBaseCenter, float radius, float height, const AABB& box);
}

#endif // SCENE_OBJECT_HPP