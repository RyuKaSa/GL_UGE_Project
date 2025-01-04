// scene_object.cpp
#include "scene_object.hpp"

namespace utils_scene
{

    // **Define** the map for planet spiral parameters
    std::map<std::string, PlanetSpiralParams> planetSpiralParameters;

    // Base Values
    const float BASE_EARTH_SPEED = 0.2f;  // Base speed for Earth revolution (arbitrary)
    const float MIN_DISTANCE = 2.0f;      // Minimum scaled distance (Mercury)
    const float MAX_DISTANCE = 10.0f;     // Maximum scaled distance (Neptune)
    const float MIN_HEIGHT = -5.0f;       // Minimum height offset
    const float MAX_HEIGHT = -30.0f;      // Maximum height offset

    // Relative orbital speed multipliers (scaled from Earth's speed)
    std::map<std::string, float> speedScale = {
        {"mercury", 4.15f},
        {"venus", 1.63f},
        {"venus_atmosphere", 1.63f},
        {"earth", 1.0f},
        {"earth_atmosphere", 1.0f},
        {"mars", 0.63f},
        {"jupiter", 0.484f},
        {"saturn", 0.334f},
        {"saturn_ring", 0.334f},
        {"uranus", 0.112f},
        {"neptune", 0.066f}
    };

    // Relative distance multipliers (scaled between MIN_DISTANCE and MAX_DISTANCE)
    std::map<std::string, float> distanceScale = {
        {"mercury", 0.0f},  // Closest to MIN_DISTANCE
        {"venus", 0.2f},
        {"venus_atmosphere", 0.2f},
        {"earth", 0.3f},
        {"earth_atmosphere", 0.3f},
        {"mars", 0.4f},
        {"jupiter", 0.6f},
        {"saturn", 0.8f},
        {"saturn_ring", 0.8f},
        {"uranus", 0.9f},
        {"neptune", 1.0f}   // Farthest at MAX_DISTANCE
    };

    // **Initialize Parameters**
    void initializePlanetSpiralParameters() {
        for (auto it = speedScale.begin(); it != speedScale.end(); ++it) {
            const std::string& planet = it->first;
            float speed = it->second;

            // Interpolate distance and height directly
            float scaledDistance = MIN_DISTANCE + (MAX_DISTANCE - MIN_DISTANCE) * distanceScale[planet];
            float scaledHeight = MIN_HEIGHT + (MAX_HEIGHT - MIN_HEIGHT) * distanceScale[planet];

            planetSpiralParameters[planet] = {
                scaledDistance,               // Scaled Radius
                BASE_EARTH_SPEED * speed,     // Scaled Speed
                scaledHeight                  // Scaled Height
            };
        }
    }

    // **Update planet positions dynamically**
    void updatePlanetPositions(float currentFrame, const glm::vec3& spiralCenter) {
        for (const auto& pair : planetSpiralParameters) {
            const std::string& planetName = pair.first;
            const PlanetSpiralParams& params = pair.second;

            // Calculate new position based on spiral motion
            glm::vec3 newPosition;
            newPosition.x = spiralCenter.x + params.spiralRadius * cos(currentFrame * params.spiralSpeed);
            newPosition.y = spiralCenter.y + params.fixedHeight;
            newPosition.z = spiralCenter.z + params.spiralRadius * sin(currentFrame * params.spiralSpeed);

            // Apply the new position
            setObjectPosition(planetName, newPosition);
        }
    }

    void updateDisplayPlanetPositions(float currentFrame) {
        // Fixed rotation speed for display planets
        const float ROTATION_SPEED = 1.3f;
        const glm::vec3 ROTATION_AXIS = glm::vec3(0.5f, 0.5f, 1.0f);

        // List of planet display names
        std::vector<std::string> displayPlanets = {
            "mercury_display", "venus_display", "venus_atmosphere_display",
            "earth_display", "earth_atmosphere_display", "mars_display",
            "jupiter_display", "saturn_display", "saturn_ring_display",
            "uranus_display", "neptune_display"
        };

        // Apply rotation to each display planet
        for (const auto &planetName : displayPlanets) {
            float rotationAngle = currentFrame * ROTATION_SPEED; // Calculate angle dynamically
            setObjectRotation(planetName, ROTATION_AXIS, rotationAngle);
        }
    }

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
        if (name == "saturn_ring" || name == "saturn_ring_display") {
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

    // setObjectPosition
    void setObjectPosition(const std::string &name, const glm::vec3 &position)
    {
        for (auto &obj : sceneObjects)
        {
            if (obj.name == name)
            {
                obj.position = position;
            }
        }
        // for transparent objects
        for (auto &obj : sceneObjectsTransparent)
        {
            if (obj.name == name)
            {
                obj.position = position;
            }
        }
    }

    // set obect rotation
    void setObjectRotation(const std::string &name, const glm::vec3 &rotationAxis, float rotationAngle)
    {
        for (auto &obj : sceneObjects)
        {
            if (obj.name == name)
            {
                obj.rotationAxis = rotationAxis;
                obj.rotationAngle = rotationAngle;
            }
        }
        // for transparent objects
        for (auto &obj : sceneObjectsTransparent)
        {
            if (obj.name == name)
            {
                obj.rotationAxis = rotationAxis;
                obj.rotationAngle = rotationAngle;
            }
        }
    }

} // namespace utils_scene