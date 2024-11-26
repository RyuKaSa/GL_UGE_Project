#include "scene_object.hpp"

std::vector<SceneObject> sceneObjects;

void addCube(
    const std::string& name,
    const glm::vec3& position,
    const glm::vec3& scale,
    const glm::vec3& color,
    bool useTexture,
    GLuint textureID,
    GLuint normalMapID,
    const glm::vec3& rotationAxis,
    float rotationAngle,
    GLuint vaoID,
    GLsizei indexCount,
    bool isStatic
) {
    SceneObject cube;
    cube.name = name;
    cube.type = ObjectType::Cube;
    cube.position = position;
    cube.initialPosition = position;
    cube.scale = scale;
    cube.color = color;
    cube.useTexture = useTexture;
    cube.textureID = textureID;
    cube.normalMapID = normalMapID;
    cube.rotationAxis = rotationAxis;
    cube.rotationAngle = rotationAngle;
    cube.vaoID = vaoID;
    cube.indexCount = indexCount;
    cube.isStatic = isStatic;

    glm::vec3 halfSize = scale * 0.5f;
    cube.boundingBox.min = position - halfSize;
    cube.boundingBox.max = position + halfSize;

    sceneObjects.push_back(cube);
}

void addSphere(
    const std::string& name,
    const glm::vec3& position,
    float radius,
    const glm::vec3& color,
    bool useTexture,
    GLuint textureID,
    GLuint normalMapID,
    GLuint vaoID,
    GLsizei vertexCount,
    bool isStatic
) {
    SceneObject sphereObject;
    sphereObject.name = name;
    sphereObject.type = ObjectType::Sphere;
    sphereObject.position = position;
    sphereObject.initialPosition = position;
    sphereObject.scale = glm::vec3(radius);
    sphereObject.color = color;
    sphereObject.useTexture = useTexture;
    sphereObject.textureID = textureID;
    sphereObject.normalMapID = normalMapID;
    sphereObject.vaoID = vaoID;
    sphereObject.indexCount = vertexCount;
    sphereObject.isStatic = isStatic;

    sphereObject.boundingBox.min = position - glm::vec3(radius);
    sphereObject.boundingBox.max = position + glm::vec3(radius);

    sceneObjects.push_back(sphereObject);
}

void createCompositeCube(
    const std::string& baseName,
    const glm::vec3& origin,
    const glm::vec3& size,
    GLuint textureID,
    GLuint normalMapID,
    GLuint vaoID,
    GLsizei indexCount,
    bool isStatic
) {
    int numCubesX = static_cast<int>(size.x);
    int numCubesY = static_cast<int>(size.y);
    int numCubesZ = static_cast<int>(size.z);

    for (int x = 0; x < numCubesX; ++x) {
        for (int y = 0; y < numCubesY; ++y) {
            for (int z = 0; z < numCubesZ; ++z) {
                glm::vec3 position = origin + glm::vec3(x, y, z);
                std::string name = baseName + "_" + std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
                addCube(
                    name,                       // Name
                    position,                   // Position
                    glm::vec3(1.0f),            // Scale (individual cubes are 1x1x1 units)
                    glm::vec3(1.0f),            // Color (white, not used if texture is applied)
                    true,                       // Use texture
                    textureID,                  // Texture ID
                    normalMapID,                // Normal map ID
                    glm::vec3(0.0f),            // Rotation axis
                    0.0f,                       // Rotation angle
                    vaoID,                      // VAO ID
                    indexCount,                 // Index count
                    isStatic                    // Is static
                );
            }
        }
    }
}

void addModel(
    const std::string& name,
    const glm::vec3& position,
    const glm::vec3& scale,
    bool useTexture,
    GLuint textureID,
    GLuint normalMapID,
    GLuint vaoID,
    GLsizei indexCount,
    const AABB& boundingBox,
    const glm::vec3& rotationAxis,
    float rotationAngle,
    bool isStatic
) {
    SceneObject obj;
    obj.name = name;
    obj.type = ObjectType::Model;
    obj.position = position;
    obj.initialPosition = position;
    obj.scale = scale;
    obj.color = glm::vec3(1.0f); // Default color white
    obj.useTexture = useTexture;
    obj.textureID = textureID;
    obj.normalMapID = normalMapID;
    obj.rotationAxis = rotationAxis;
    obj.rotationAngle = rotationAngle;
    obj.boundingBox = boundingBox;
    obj.vaoID = vaoID;
    obj.indexCount = indexCount;
    obj.isStatic = isStatic;

    sceneObjects.push_back(obj);
}