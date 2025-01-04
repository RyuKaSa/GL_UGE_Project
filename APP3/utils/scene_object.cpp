// scene_object.cpp
#include "scene_object.hpp"

namespace utils_scene
{

    std::vector<SceneObject> sceneObjects;
    std::vector<SceneObject> sceneObjectsTransparent;
    std::vector<SceneObject> sceneObjectsSkybox;

    void addCube(const std::string &name,
                const glm::vec3 &position,
                const glm::vec3 &scale,
                const Material &material,
                const glm::vec3 &rotationAxis,
                float rotationAngle,
                GLuint vaoID,
                GLsizei indexCount,
                bool isStatic)
    {
        SceneObject cube;
        cube.name = name;
        cube.type = ObjectType::Cube;
        cube.position = position;
        cube.initialPosition = position;
        cube.scale = scale;
        cube.rotationAxis = rotationAxis;
        cube.rotationAngle = rotationAngle;
        cube.vaoID = vaoID;
        cube.indexCount = indexCount;
        cube.isStatic = isStatic;

        // Calculate bounding box
        glm::vec3 halfSize = scale * 0.5f;
        cube.boundingBox.min = position - halfSize;
        cube.boundingBox.max = position + halfSize;

        // Ensure material is reused
        int materialIndex = MaterialManager::getInstance().findMaterial(material);
        if (materialIndex == -1) {
            materialIndex = MaterialManager::getInstance().addOrGetMaterial(material);
        }
        cube.materialIndex = materialIndex;

        sceneObjects.push_back(cube);
    }

    void addTransparentCube(const std::string &name,
                            const glm::vec3 &position,
                            const glm::vec3 &scale,
                            const Material &material,
                            const glm::vec3 &rotationAxis,
                            float rotationAngle,
                            GLuint vaoID,
                            GLsizei indexCount,
                            bool isStatic)
    {
        SceneObject cube;
        cube.name = name;
        cube.type = ObjectType::Cube;
        cube.position = position;
        cube.initialPosition = position;
        cube.scale = scale;
        cube.rotationAxis = rotationAxis;
        cube.rotationAngle = rotationAngle;
        cube.vaoID = vaoID;
        cube.indexCount = indexCount;
        cube.isStatic = isStatic;

        // Calculate bounding box
        glm::vec3 halfSize = scale * 0.5f;
        cube.boundingBox.min = position - halfSize;
        cube.boundingBox.max = position + halfSize;

        // Assign material using MaterialManager
        int materialIndex = MaterialManager::getInstance().findMaterial(material);
        if (materialIndex == -1) {
            materialIndex = MaterialManager::getInstance().addOrGetMaterial(material);
        }
        cube.materialIndex = materialIndex;

        sceneObjectsTransparent.push_back(cube);
    }

    void addSphere(const std::string &name,
                   const glm::vec3 &position,
                   float radius,
                   const Material &material,
                   GLuint vaoID,
                   GLsizei vertexCount,
                   bool isStatic)
    {
        SceneObject sphereObject;
        sphereObject.name = name;
        sphereObject.type = ObjectType::Sphere;
        sphereObject.position = position;
        sphereObject.initialPosition = position;
        sphereObject.scale = glm::vec3(radius);
        sphereObject.rotationAxis = glm::vec3(0.0f);
        sphereObject.rotationAngle = 0.0f;
        sphereObject.vaoID = vaoID;
        sphereObject.indexCount = vertexCount;
        sphereObject.isStatic = isStatic;


        // Assign material using MaterialManager
        int materialIndex = MaterialManager::getInstance().findMaterial(material);
        if (materialIndex == -1) {
            materialIndex = MaterialManager::getInstance().addOrGetMaterial(material);
        }
        sphereObject.materialIndex = materialIndex;

        sceneObjects.push_back(sphereObject);
    }

    // transparent sphere
    void addTransparentSphere(const std::string &name,
                   const glm::vec3 &position,
                   float radius,
                   const Material &material,
                   GLuint vaoID,
                   GLsizei vertexCount,
                   bool isStatic)
    {
        SceneObject sphereObject;
        sphereObject.name = name;
        sphereObject.type = ObjectType::Sphere;
        sphereObject.position = position;
        sphereObject.initialPosition = position;
        // if name is saturn_ring, override the scale, keep the radius for the just the radius, and squish the ring
        if (name == "saturn_ring") {
            sphereObject.scale = glm::vec3(radius, 0.01f, radius);
        } else {
            sphereObject.scale = glm::vec3(radius);
        }
        sphereObject.rotationAxis = glm::vec3(0.0f);
        sphereObject.rotationAngle = 0.0f;
        sphereObject.vaoID = vaoID;
        sphereObject.indexCount = vertexCount;
        sphereObject.isStatic = isStatic;

        // Assign material using MaterialManager
        int materialIndex = MaterialManager::getInstance().findMaterial(material);
        if (materialIndex == -1) {
            materialIndex = MaterialManager::getInstance().addOrGetMaterial(material);
        }
        sphereObject.materialIndex = materialIndex;

        sceneObjectsTransparent.push_back(sphereObject);
    }
    
    // add sphere no bounding box
    void addSkySphere(const std::string &name,
                   const glm::vec3 &position,
                   float radius,
                   const Material &material,
                   GLuint vaoID,
                   GLsizei vertexCount,
                   bool isStatic)
    {
        SceneObject sphereObject;
        sphereObject.name = name;
        sphereObject.type = ObjectType::Sphere;
        sphereObject.position = position;
        sphereObject.initialPosition = position;
        sphereObject.scale = glm::vec3(radius);
        sphereObject.rotationAxis = glm::vec3(0.0f);
        sphereObject.rotationAngle = 0.0f;
        sphereObject.vaoID = vaoID;
        sphereObject.indexCount = vertexCount;
        sphereObject.isStatic = isStatic;

        // Assign material using MaterialManager
        int materialIndex = MaterialManager::getInstance().findMaterial(material);
        if (materialIndex == -1) {
            materialIndex = MaterialManager::getInstance().addOrGetMaterial(material);
        }
        sphereObject.materialIndex = materialIndex;

        sceneObjectsSkybox.push_back(sphereObject);
        // sceneObjects.push_back(sphereObject);
        // print object name
        std::cout << "Skybox sphere name: " << sphereObject.name << std::endl;
    }

    void createCompositeCube(const std::string &name,
                             const glm::vec3 &origin,
                             const glm::vec3 &size,
                             const Material &material,
                             GLuint vaoID,
                             GLsizei indexCount,
                             bool isStatic)
    {
        int numCubesX = static_cast<int>(size.x);
        int numCubesY = static_cast<int>(size.y);
        int numCubesZ = static_cast<int>(size.z);

        for (int x = 0; x < numCubesX; ++x)
        {
            for (int y = 0; y < numCubesY; ++y)
            {
                for (int z = 0; z < numCubesZ; ++z)
                {
                    glm::vec3 position = origin + glm::vec3(x, y, z);
                    std::string cubeName = name + "_" + std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
                    addCube(
                        cubeName,        // Name
                        position,        // Position
                        glm::vec3(1.0f), // Scale (individual cubes are 1x1x1 units)
                        material,        // Material
                        glm::vec3(0.0f), // Rotation axis
                        0.0f,            // Rotation angle
                        vaoID,           // VAO ID
                        indexCount,      // Index count
                        isStatic         // Is static
                    );
                }
            }
        }
    }

    void createTransparentCompositeCube(const std::string &name,
                                        const glm::vec3 &origin,
                                        const glm::vec3 &size,
                                        const Material &material,
                                        GLuint vaoID,
                                        GLsizei indexCount,
                                        bool isStatic)
    {
        int numCubesX = static_cast<int>(size.x);
        int numCubesY = static_cast<int>(size.y);
        int numCubesZ = static_cast<int>(size.z);

        for (int x = 0; x < numCubesX; ++x)
        {
            for (int y = 0; y < numCubesY; ++y)
            {
                for (int z = 0; z < numCubesZ; ++z)
                {
                    glm::vec3 position = origin + glm::vec3(x, y, z);
                    std::string cubeName = name + "_" + std::to_string(x) + "_" + std::to_string(y) + "_" + std::to_string(z);
                    addTransparentCube(
                        cubeName,        // Name
                        position,        // Position
                        glm::vec3(1.0f), // Scale (individual cubes are 1x1x1 units)
                        material,        // Material
                        glm::vec3(0.0f), // Rotation axis
                        0.0f,            // Rotation angle
                        vaoID,           // VAO ID
                        indexCount,      // Index count
                        isStatic         // Is static
                    );
                }
            }
        }
    }

    void addModel(const std::string &name,
                  const glm::vec3 &position,
                  const glm::vec3 &scale,
                  const Material &material,
                  GLuint vaoID,
                  GLsizei indexCount,
                  const AABB &boundingBox,
                  const glm::vec3 &rotationAxis,
                  float rotationAngle,
                  bool isStatic)
    {
        SceneObject obj;
        obj.name = name;
        obj.type = ObjectType::Model;
        obj.position = position;
        obj.initialPosition = position;
        obj.scale = scale;
        obj.rotationAxis = rotationAxis;
        obj.rotationAngle = rotationAngle;
        obj.vaoID = vaoID;
        obj.indexCount = indexCount;
        obj.isStatic = isStatic;

        obj.boundingBox = boundingBox;

        // Assign material using MaterialManager
        int materialIndex = MaterialManager::getInstance().findMaterial(material);
        if (materialIndex == -1) {
            materialIndex = MaterialManager::getInstance().addOrGetMaterial(material);
        }
        obj.materialIndex = materialIndex;

        sceneObjects.push_back(obj);
    }

    // getTransparentObjectPosition
    glm::vec3 getTransparentObjectPosition(const std::string &name)
    {
        for (const auto &obj : sceneObjectsTransparent)
        {
            if (obj.name == name)
            {
                return obj.position;
            }
        }
        return glm::vec3(0.0f);
    }

} // namespace utils_scene