#include "scene_object.hpp"

AABB computeAABB(const std::vector<float>& vertices) {
    if (vertices.empty()) {
        return AABB(glm::vec3(0.0f), glm::vec3(0.0f));
    }
    
    glm::vec3 minVertex(vertices[0], vertices[1], vertices[2]);
    glm::vec3 maxVertex = minVertex;
    
    for (size_t i = 3; i < vertices.size(); i += 3) {
        glm::vec3 vertex(vertices[i], vertices[i + 1], vertices[i + 2]);
        minVertex = glm::min(minVertex, vertex);
        maxVertex = glm::max(maxVertex, vertex);
    }
    
    return AABB(minVertex, maxVertex);
}

namespace SceneObjectManager {

    // we will have 3 instances of the std::vector<SceneObject> sceneObjects;
    //  - one for the dynamic objects
    //  - one for the visible objects
    //  - one for ALL scene objects
    
    std::vector<SceneObject> allObjects;
    std::vector<SceneObject*> dynamicObjects;
    std::vector<SceneObject*> visibleObjects;

    void addCube(const std::string& name, const glm::vec3& position, const glm::vec3& scale, const glm::vec3& color, bool useTexture, GLuint textureID, GLuint normalMapID, const glm::vec3& rotationAxis, float rotationAngle, GLuint vaoID, GLsizei indexCount, bool isStatic) {
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

    bool checkCollision(const glm::vec3& cylinderBaseCenter, float radius, float height, const AABB& box) {
        float horizontalDistanceSquared = 0.0f;

        for (int i = 0; i < 3; i += 2) {
            if (cylinderBaseCenter[i] < box.min[i]) {
                horizontalDistanceSquared += (box.min[i] - cylinderBaseCenter[i]) * (box.min[i] - cylinderBaseCenter[i]);
            } else if (cylinderBaseCenter[i] > box.max[i]) {
                horizontalDistanceSquared += (cylinderBaseCenter[i] - box.max[i]) * (cylinderBaseCenter[i] - box.max[i]);
            }
        }

        if (horizontalDistanceSquared > (radius * radius)) {
            return false;
        }

        float cylinderTop = cylinderBaseCenter.y + height / 2.0f;
        float cylinderBottom = cylinderBaseCenter.y - height / 2.0f;
        if (cylinderTop < box.min.y || cylinderBottom > box.max.y) {
            return false;
        }

        return true;
    }

}