#include "scene_object.hpp"

std::vector<SceneObject> sceneObjects;

void addCube(const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color, bool useTexture, GLuint textureID, GLuint normalMapID, const glm::vec3& rotationAxis, float rotationAngle, GLuint vaoID, GLsizei indexCount) {
    SceneObject cube;
    cube.type = ObjectType::Cube;
    cube.position = position;
    cube.scale = scale;
    cube.color = color;
    cube.useTexture = useTexture;
    cube.textureID = textureID;
    cube.normalMapID = normalMapID;
    cube.rotationAxis = rotationAxis;
    cube.rotationAngle = rotationAngle;
    cube.vaoID = vaoID;
    cube.indexCount = indexCount;

    glm::vec3 halfSize = scale * 0.5f;
    cube.boundingBox.min = position - halfSize;
    cube.boundingBox.max = position + halfSize;

    sceneObjects.push_back(cube);
}

void addSphere(const glm::vec3& position, float radius, const glm::vec3& color, bool useTexture, GLuint textureID, GLuint normalMapID, GLuint vaoID, GLsizei vertexCount) {
    SceneObject sphereObject;
    sphereObject.type = ObjectType::Sphere;
    sphereObject.position = position;
    sphereObject.scale = glm::vec3(radius);
    sphereObject.color = color;
    sphereObject.useTexture = useTexture;
    sphereObject.textureID = textureID;
    sphereObject.normalMapID = normalMapID;
    sphereObject.vaoID = vaoID;
    sphereObject.indexCount = vertexCount;

    sphereObject.boundingBox.min = position - glm::vec3(radius);
    sphereObject.boundingBox.max = position + glm::vec3(radius);

    sceneObjects.push_back(sphereObject);
}