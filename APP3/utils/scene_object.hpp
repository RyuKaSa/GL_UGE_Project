// scene_object.hpp
#ifndef SCENE_OBJECT_HPP
#define SCENE_OBJECT_HPP

#include "utilities.hpp"
#include "global.hpp"
#include "material.hpp"  
#include "material_manager.hpp"
#include <map>
#include <vector>
#include <string>

namespace utils_scene
{

    enum class ObjectType
    {
        Cube,
        Sphere,
        Model
    };

// Define a struct to hold spiral parameters for planets
    struct PlanetSpiralParams {
        float spiralRadius;
        float spiralSpeed;
        float fixedHeight;
    };

    // Map to hold parameters for each planet
    extern std::map<std::string, PlanetSpiralParams> planetSpiralParameters;

    // Function to initialize planet spiral parameters
    void initializePlanetSpiralParameters();

    // Function to update planet positions
    void updatePlanetPositions(float currentFrame, const glm::vec3& spiralCenter);
    
    struct SceneObject
    {
        std::string name;
        ObjectType type;
        glm::vec3 position;
        glm::vec3 initialPosition;
        glm::vec3 scale;
        glm::vec3 rotationAxis;
        float rotationAngle;
        AABB boundingBox;
        GLuint vaoID;
        GLsizei indexCount;
        bool isStatic;

        // Material reference
        int materialIndex; // Index into MaterialManager's material list

        // Constructor
        SceneObject()
            : position(0.0f), initialPosition(0.0f), scale(1.0f),
              rotationAxis(0.0f), rotationAngle(0.0f), vaoID(0),
              indexCount(0), isStatic(false), materialIndex(-1) {}
    };

    extern std::vector<SceneObject> sceneObjects;
    extern std::vector<SceneObject> sceneObjectsTransparent;
    extern std::vector<SceneObject> sceneObjectsSkybox;

    // Updated function declarations to accept Material
    void addCube(const std::string &name,
                 const glm::vec3 &position,
                 const glm::vec3 &scale,
                 const Material &material,
                 const glm::vec3 &rotationAxis = glm::vec3(0.0f),
                 float rotationAngle = 0.0f,
                 GLuint vaoID = 0,
                 GLsizei indexCount = 0,
                 bool isStatic = false);

    void addTransparentCube(const std::string &name,
                            const glm::vec3 &position,
                            const glm::vec3 &scale,
                            const Material &material,
                            const glm::vec3 &rotationAxis = glm::vec3(0.0f),
                            float rotationAngle = 0.0f,
                            GLuint vaoID = 0,
                            GLsizei indexCount = 0,
                            bool isStatic = false);

    void addSphere(const std::string &name,
                   const glm::vec3 &position,
                   float radius,
                   const Material &material,
                   GLuint vaoID = 0,
                   GLsizei vertexCount = 0,
                   bool isStatic = false);

    void addTransparentSphere(const std::string &name,
                      const glm::vec3 &position,
                      float radius,
                      const Material &material,
                      GLuint vaoID = 0,
                      GLsizei vertexCount = 0,
                      bool isStatic = false);

    void addSkySphere(const std::string &name,
                      const glm::vec3 &position,
                      float radius,
                      const Material &material,
                      GLuint vaoID = 0,
                      GLsizei vertexCount = 0,
                      bool isStatic = false);

    void createCompositeCube(const std::string &name,
                             const glm::vec3 &origin,
                             const glm::vec3 &size,
                             const Material &material,
                             GLuint vaoID,
                             GLsizei indexCount,
                             bool isStatic);

    void createTransparentCompositeCube(const std::string &name,
                                        const glm::vec3 &origin,
                                        const glm::vec3 &size,
                                        const Material &material,
                                        GLuint vaoID,
                                        GLsizei indexCount,
                                        bool isStatic);

    void addModel(const std::string &name,
                  const glm::vec3 &position,
                  const glm::vec3 &scale,
                  const Material &material,
                  GLuint vaoID,
                  GLsizei indexCount,
                  const AABB &boundingBox,
                  const glm::vec3 &rotationAxis = glm::vec3(0.0f),
                  float rotationAngle = 0.0f,
                  bool isStatic = false);

    // getTransparentObjectPosition
    glm::vec3 getTransparentObjectPosition(const std::string &name);

    // setObjectPosition
    void setObjectPosition(const std::string &name, const glm::vec3 &position);

} // namespace utils_scene

#endif // SCENE_OBJECT_HPP