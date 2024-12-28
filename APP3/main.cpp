#include "utils/global.hpp"
#include "utils/cube.hpp"
#include "utils/sphere.hpp"
#include "utils/scene_object.hpp"
#include "utils/texture.hpp"
#include "utils/collision.hpp"
#include "utils/utilities.hpp"
#include "utils/resource_loader.hpp"
#include "utils/models.hpp"
#include "utils/initialization.hpp"
#include "utils/shader.hpp"
#include "utils/rendering.hpp"
#include "utils/lights.hpp"

#include <src/stb_image.h>

#include <src/tiny_obj_loader.h>

#include <glad/glad.h>
#include <iostream>
#include <filesystem>
#include <glimac/Program.hpp>
#include <glimac/Image.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <SDL2/SDL.h>
#include <cstddef> // For offsetof
#include <vector>
#include <map>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp> // For vector calculations

int main(int argc, char *argv[])
{
    (void)argc;

    auto windowManager = utils_init::initOpenGL(window_width, window_height);

    /*********************************
     * Initialization code
     *********************************/

    // Sphere setup
    glimac::Sphere sphere(1, 32, 16);
    std::vector<SphereVertex> sphereVertices;

    size_t sphereVertexCount = utils_object::createSphereVertices(sphereVertices, sphere);
    utils_object::computeSphereTangents(sphereVertices);

    GLuint sphereVBO, sphereVAO;
    utils_object::setupSphereBuffers(sphereVertices, sphereVBO, sphereVAO);

    GLsizei sphereVertexCountGL = static_cast<GLsizei>(sphereVertexCount);

    // Cube setup
    // Create cube vertices and indices
    std::vector<Vertex3D> cubeVertices;
    std::vector<GLuint> cubeIndices;
    utils_object::createCube(cubeVertices, cubeIndices);
    utils_object::computeCubeTangents(cubeVertices, cubeIndices);

    // Set up VBO, EBO, and VAO for the cube
    GLuint cubeVBO, cubeEBO, cubeVAO;
    utils_object::setupCubeBuffers(cubeVertices, cubeIndices, cubeVBO, cubeEBO, cubeVAO);
    std::cout << "Cube VBO, EBO, and VAO set up" << std::endl;

    glimac::FilePath applicationPath(argv[0]);

    // Load shaders
    utils_loader::Shader room1(
        applicationPath.dirPath() + "APP3/shaders/room1.vs.glsl",
        applicationPath.dirPath() + "APP3/shaders/room1.fs.glsl"
    );

    // room 2 is to be acticaved when camera position is in the second room, i.e., x > 20.5, no condition on z or y
    utils_loader::Shader room2(
        applicationPath.dirPath() + "APP3/shaders/room2.vs.glsl",
        applicationPath.dirPath() + "APP3/shaders/room2.fs.glsl"
    );

    utils_loader::Shader depthShader(
        applicationPath.dirPath() + "APP3/shaders/point_shadow_depth.vs.glsl",
        applicationPath.dirPath() + "APP3/shaders/point_shadow_depth.fs.glsl"
    );

    // Check shaders
    if (room1.getID() == 0 || room2.getID() == 0 || depthShader.getID() == 0) {
        std::cerr << "Failed to compile/link one or more shaders. Exiting." << std::endl;
        return -1;
    }

    room1.use();
    std::cout << "Unified shader program in use" << std::endl;

    // Load textures
    GLuint textureID, stoneTextureID, brownTerracottaTextureID, soccerTextureID;
    GLuint textureID_normalMap, stoneTextureID_normalMap, brownTerracottaTextureID_normalMap, soccerTextureID_normalMap;
    GLuint chairBaseColorTextureID, chairNormalMapTextureID;
    utils_loader::loadTextures(textureID, stoneTextureID, brownTerracottaTextureID, soccerTextureID,
                textureID_normalMap, stoneTextureID_normalMap, brownTerracottaTextureID_normalMap, soccerTextureID_normalMap,
                chairBaseColorTextureID, chairNormalMapTextureID, 
                applicationPath);

    GLuint depthCubeMap, shadowMapFBO;
    utils_loader::setupDepthCubeMap(depthCubeMap, shadowMapFBO);

    // Check depth shader uniforms
    std::cout << "Checking depth shader uniforms..." << std::endl;

    GLint uDepth_LightSpaceMatrixLocation = depthShader.getUniformLocation("uLightSpaceMatrix");
    if (uDepth_LightSpaceMatrixLocation == -1)
        std::cerr << "Failed to get 'uLightSpaceMatrix' location in depth shader" << std::endl;

    GLint uDepth_ModelMatrixLocation = depthShader.getUniformLocation("model");
    if (uDepth_ModelMatrixLocation == -1)
        std::cerr << "Failed to get 'model' location in depth shader" << std::endl;

    // Get uniform location for model matrix in unified shader
    GLint uModelMatrixLocation = room1.getUniformLocation("uModelMatrix");
    if (uModelMatrixLocation == -1)
        std::cerr << "Failed to get 'uModelMatrix' location in unified shader" << std::endl;

    // Get uniform locations
    room1.use();
    std::cout << "Shader program in use" << std::endl;

    // Get uniform locations
    GLint uMVPMatrixLocation = glGetUniformLocation(room1.getGLId(), "uMVPMatrix");
    GLint uMVMatrixLocation = glGetUniformLocation(room1.getGLId(), "uMVMatrix");
    GLint uNormalMatrixLocation = glGetUniformLocation(room1.getGLId(), "uNormalMatrix");
    GLint uTextureLocation = glGetUniformLocation(room1.getGLId(), "uTexture");
    GLint uUseTextureLocation = glGetUniformLocation(room1.getGLId(), "uUseTexture");

    GLint uKdLocation = glGetUniformLocation(room1.getGLId(), "uKd");
    GLint uKsLocation = glGetUniformLocation(room1.getGLId(), "uKs");
    GLint uShininessLocation = glGetUniformLocation(room1.getGLId(), "uShininess");
    GLint uLightPos_vsLocation = glGetUniformLocation(room1.getGLId(), "uLightPos_vs");
    GLint uLightIntensityLocation = glGetUniformLocation(room1.getGLId(), "uLightIntensity");

    // Sanity check
    if (uMVPMatrixLocation == -1)
        std::cerr << "Failed to get 'uMVPMatrix' location" << std::endl;
    if (uMVMatrixLocation == -1)
        std::cerr << "Failed to get 'uMVMatrix' location" << std::endl;
    if (uNormalMatrixLocation == -1)
        std::cerr << "Failed to get 'uNormalMatrix' location" << std::endl;
    if (uTextureLocation == -1)
        std::cerr << "Failed to get 'uTexture' location" << std::endl;
    if (uUseTextureLocation == -1)
        std::cerr << "Failed to get 'uUseTexture' location" << std::endl;
    if (uKdLocation == -1)
        std::cerr << "Failed to get 'uKd' location" << std::endl;
    if (uKsLocation == -1)
        std::cerr << "Failed to get 'uKs' location" << std::endl;
    if (uShininessLocation == -1)
        std::cerr << "Failed to get 'uShininess' location" << std::endl;
    // if (uLightDir_vsLocation == -1)
    //     std::cerr << "Failed to get 'uLightDir_vs' location" << std::endl;
    if (uLightIntensityLocation == -1)
        std::cerr << "Failed to get 'uLightIntensity' location" << std::endl;

    // Retrieve uniform locations for room2
    room2.use();
    std::cout << "Room2 shader program in use" << std::endl;

    // Get uniform locations for room2
    GLint room2_uMVPMatrixLocation = glGetUniformLocation(room2.getGLId(), "uMVPMatrix");
    GLint room2_uMVMatrixLocation = glGetUniformLocation(room2.getGLId(), "uMVMatrix");
    GLint room2_uNormalMatrixLocation = glGetUniformLocation(room2.getGLId(), "uNormalMatrix");
    GLint room2_uTextureLocation = glGetUniformLocation(room2.getGLId(), "uTexture");
    GLint room2_uUseTextureLocation = glGetUniformLocation(room2.getGLId(), "uUseTexture");

    GLint room2_uKdLocation = glGetUniformLocation(room2.getGLId(), "uKd");
    GLint room2_uKsLocation = glGetUniformLocation(room2.getGLId(), "uKs");
    GLint room2_uShininessLocation = glGetUniformLocation(room2.getGLId(), "uShininess");
    GLint room2_uLightPos_vsLocation = glGetUniformLocation(room2.getGLId(), "uLightPos_vs");
    GLint room2_uLightIntensityLocation = glGetUniformLocation(room2.getGLId(), "uLightIntensity");

    // Sanity check for room2 uniforms
    if (room2_uMVPMatrixLocation == -1)
        std::cerr << "Failed to get 'uMVPMatrix' location in room2 shader" << std::endl;
    if (room2_uMVMatrixLocation == -1)
        std::cerr << "Failed to get 'uMVMatrix' location in room2 shader" << std::endl;
    if (room2_uNormalMatrixLocation == -1)
        std::cerr << "Failed to get 'uNormalMatrix' location in room2 shader" << std::endl;
    if (room2_uTextureLocation == -1)
        std::cerr << "Failed to get 'uTexture' location in room2 shader" << std::endl;
    if (room2_uUseTextureLocation == -1)
        std::cerr << "Failed to get 'uUseTexture' location in room2 shader" << std::endl;
    if (room2_uKdLocation == -1)
        std::cerr << "Failed to get 'uKd' location in room2 shader" << std::endl;
    if (room2_uKsLocation == -1)
        std::cerr << "Failed to get 'uKs' location in room2 shader" << std::endl;
    if (room2_uShininessLocation == -1)
        std::cerr << "Failed to get 'uShininess' location in room2 shader" << std::endl;
    if (room2_uLightIntensityLocation == -1)
        std::cerr << "Failed to get 'uLightIntensity' location in room2 shader" << std::endl;

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    std::cout << "Depth test enabled" << std::endl;

    // Set the clear color to a dark gray
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Light properties
    float spiralRadius = 2.0f;  // Radius of the spiral
    float spiralSpeed = 0.5f;   // Speed of the spiral movement
    float fixedHeight = 1.5f;   // Height variation of the light

    // Light space matrix for shadows
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.0f);
    glm::mat4 lightView = glm::lookAt(glm::vec3(5.0f, 10.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    // Setup projection matrix for cube map
    float nearPlane = 1.0f;
    float farPlane = 25.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);

    // Camera parameters
    glm::vec3 cameraPos(1.0f, 2.0f, 1.0f);    // Initial position of the camera
    glm::vec3 cameraFront(0.0f, 0.0f, -1.0f); // Direction the camera is looking at
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);     // Up vector

    float cameraPosY = cameraPos.y; // Store the initial y position

    float yaw = -90.0f; // Yaw angle initialized to -90 degrees to look towards negative Z
    float pitch = 0.0f; // Pitch angle

    float cameraSpeed = 10.0f; // Adjust accordingly
    float deltaTime = 0.0f;   // Time between current frame and last frame
    float lastFrame = 0.0f;   // Time of last frame

    // Enable relative mouse mode to capture mouse movement
    SDL_SetRelativeMouseMode(SDL_TRUE);

    int frameCount = 0;
    float fpsTimer = 0.0f;
    int fps = 0;

    // =======================
    // Scene objects creation

    // Prepare to add objects to the scene
    GLsizei cubeIndexCount = static_cast<GLsizei>(cubeIndices.size());
    // GLsizei sphereVertexCount = sphere.getVertexCount();

    // floor
    glm::vec3 origin(0.0f, 0.0f, 0.0f);
    glm::vec3 floorSize(42.0f, 1.0f, 24.0f);
    utils_scene::createCompositeCube("floor", origin, floorSize, stoneTextureID, stoneTextureID_normalMap, cubeVAO, cubeIndexCount, true);

    // wall X1
    glm::vec3 wallPosition1(0.0f, 1.0f, 0.0f);
    glm::vec3 wallSizeX(42.0f, 3.0f, 1.0f);
    utils_scene::createCompositeCube("wall_X1", wallPosition1, wallSizeX, textureID, textureID_normalMap, cubeVAO, cubeIndexCount, true);

    // wall X2
    glm::vec3 wallPosition2(0.0f, 1.0f, 23.0f);
    utils_scene::createCompositeCube("wall_X2", wallPosition2, wallSizeX, textureID, textureID_normalMap, cubeVAO, cubeIndexCount, true);

    // wall Z1
    glm::vec3 wallPosition3(0.0f, 1.0f, 1.0f);
    glm::vec3 wallSizeZ1(1.0f, 3.0f, 22.0f);
    utils_scene::createCompositeCube("wall_Z1", wallPosition3, wallSizeZ1, textureID, textureID_normalMap, cubeVAO, cubeIndexCount, true);

    // wall Z2
    glm::vec3 wallPosition4(41.0f, 1.0f, 1.0f);
    utils_scene::createCompositeCube("wall_Z2",wallPosition4, wallSizeZ1, textureID, textureID_normalMap, cubeVAO, cubeIndexCount, true);

    // separation wall, Z3
    glm::vec3 wallPosition5(20.0f, 1.0f, 1.0f);
    glm::vec3 wallSizeZ2(2.0f, 3.0f, 9.0f);
    utils_scene::createCompositeCube("wall_Z3",wallPosition5, wallSizeZ2, brownTerracottaTextureID, brownTerracottaTextureID_normalMap, cubeVAO, cubeIndexCount, true);

    // separation wall, Z4
    glm::vec3 wallPosition6(20.0f, 1.0f, 14.0f);
    utils_scene::createCompositeCube("wall_Z4",wallPosition6, wallSizeZ2, brownTerracottaTextureID, brownTerracottaTextureID_normalMap, cubeVAO, cubeIndexCount, true);

    // load a soccer ball as sophisticated object
    // Add sphere to the scene
    utils_scene::addSphere(
        "soccer_ball",               // Name
        glm::vec3(2.0f, 1.5f, 10.0f), // Position
        0.3f,                        // Radius
        glm::vec3(1.0f),             // Color (white)
        true,                        // Use texture
        soccerTextureID,             // Texture ID
        soccerTextureID_normalMap,   // Normal map ID
        sphereVAO,                   // VAO ID
        sphereVertexCount,           // Vertex count
        true                         // Is static
    );

    // Load the Heater .obj model
    utils_object::ModelData heaterModelData;
    std::string modelPath = applicationPath.dirPath() + "assets/models/HeaterOBJ/Heater.obj";
    if (!utils_object::loadOBJ(modelPath, applicationPath.dirPath() + "assets/models/HeaterOBJ/", heaterModelData)) {
        std::cerr << "Failed to load model heater" << std::endl;
    } else {
        std::cout << "Heater Model Loaded: " 
                << heaterModelData.vertices.size() / 3 << " vertices, " 
                << heaterModelData.indices.size() << " indices." << std::endl;
    }

    // Set up OpenGL buffers for the model
    setupModelBuffers(heaterModelData);

    // Compute Bounding Box for the Model
    AABB heaterModelBoundingBox = computeAABB(heaterModelData.vertices);

    // Apply scale
    glm::vec3 heaterModelScale(0.6f, 0.6f, 0.6f);
    heaterModelBoundingBox.min *= heaterModelScale;
    heaterModelBoundingBox.max *= heaterModelScale;

    // Apply translation (position)
    glm::vec3 heaterModelPosition(0.6f, 1.1f, 6.0f);
    heaterModelBoundingBox.min += heaterModelPosition;
    heaterModelBoundingBox.max += heaterModelPosition;

    // Add Heater Model to Scene Objects
    utils_scene::addModel(
        "heater",                         // Name
        heaterModelPosition,               // Position
        heaterModelScale,                  // Scale
        false,                       // Use texture
        0,
        0,                           // Normal Map ID (if any; set to 0 if none)
        heaterModelData.vao,               // VAO ID
        static_cast<GLsizei>(heaterModelData.indices.size()), // Index Count
        heaterModelBoundingBox,            // Bounding Box
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation Axis (Y-axis)
        0.0f,                        // Rotation Angle
        true
    );

    // Load the Rocking Chair model
    utils_object::ModelData rockingChairModelData;
    std::string rockingChairPath = applicationPath.dirPath() + "assets/models/Rocking_Chair/kid_rocking_chair.obj";
    std::string rockingChairBasePath = applicationPath.dirPath() + "assets/models/Rocking_Chair/Textures/";

    if (!utils_object::loadOBJ(rockingChairPath, rockingChairBasePath, rockingChairModelData)) {
        std::cerr << "Failed to load Rocking Chair model." << std::endl;
    } else {
        std::cout << "Rocking Chair Model Loaded: " 
                << rockingChairModelData.vertices.size() / 3 << " vertices, " 
                << rockingChairModelData.indices.size() << " indices." << std::endl;
    }

    // Load Rocking Chair Base Color Texture
    std::string baseColorPath = rockingChairBasePath + "/BaseColor.png";
    GLuint baseColorTextureID = utils_object::LoadTextureFromFile(baseColorPath.c_str());
    if (baseColorTextureID == 0) {
        std::cerr << "Failed to load BaseColor.png for Rocking Chair." << std::endl;
    } else {
        std::cout << "BaseColor.png for Rocking Chair loaded successfully: ID " << baseColorTextureID << std::endl;
    }

    // Load Rocking Chair Normal Map
    std::string normalMapPath = rockingChairBasePath + "/Normal.png";
    GLuint normalMapTextureID = utils_object::LoadTextureFromFile(normalMapPath.c_str());
    if (normalMapTextureID == 0) {
        std::cerr << "Failed to load Normal.png for Rocking Chair." << std::endl;
    } else {
        std::cout << "Normal.png for Rocking Chair loaded successfully: ID " << normalMapTextureID << std::endl;
    }

    // Set up OpenGL buffers for the Rocking Chair model
    setupModelBuffers(rockingChairModelData);

    // Compute Bounding Box for the Rocking Chair model
    AABB rockingChairModelBoundingBox = computeAABB(rockingChairModelData.vertices);

    // Apply scale
    glm::vec3 rockingChairModelScale(0.8f, 0.8f, 0.8f);
    rockingChairModelBoundingBox.min *= rockingChairModelScale;
    rockingChairModelBoundingBox.max *= rockingChairModelScale;

    // Apply translation (position)
    glm::vec3 rockingChairModelPosition(9.0f, 0.55f, 20.0f);
    rockingChairModelBoundingBox.min += rockingChairModelPosition;
    rockingChairModelBoundingBox.max += rockingChairModelPosition;

    // Add Rocking Chair Model to Scene Objects
    utils_scene::addModel(
        "rocking_chair",                         // Name
        rockingChairModelPosition,               // Position
        rockingChairModelScale,                  // Scale
        true,                                    // Use texture
        baseColorTextureID,                      // Texture ID (Base Color)
        normalMapTextureID,                      // Normal Map ID
        rockingChairModelData.vao,               // VAO ID
        static_cast<GLsizei>(rockingChairModelData.indices.size()), // Index Count
        rockingChairModelBoundingBox,            // Bounding Box
        glm::vec3(0.0f, 0.0f, 0.0f),             // Rotation Axis (Y-axis)
        0.0f,                                    // Rotation Angle
        false
    );

    // add the torus model
    utils_object::ModelData torusModelData;

    std::string torusPath = applicationPath.dirPath() + "assets/models/Torus/Torus.obj";
    std::string torusBasePath = applicationPath.dirPath() + "assets/models/Torus/";

    if (!utils_object::loadOBJ(torusPath, torusBasePath, torusModelData)) {
        std::cerr << "Failed to load Torus model." << std::endl;
    } else {
        std::cout << "Torus Model Loaded: "
                << torusModelData.vertices.size() / 3 << " vertices, "
                << torusModelData.indices.size() << " indices." << std::endl;
    }

    setupModelBuffers(torusModelData);

    AABB torusBoundingBox = computeAABB(torusModelData.vertices);
    // replace bounding box with new scale

    glm::vec3 torusPosition(9.0f, 1.0f, 3.0f);
    glm::vec3 torusScale(0.05f, 0.05f, 0.05f);

    torusBoundingBox.min *= torusScale;
    torusBoundingBox.max *= torusScale;

    torusBoundingBox.min += torusPosition;
    torusBoundingBox.max += torusPosition;

    utils_scene::addModel(
        "torus",                // Unique name
        torusPosition,          // Position in world space
        torusScale,             // Scale
        false,                  // useTexture? (true if you have a texture)
        0,                      // textureID if used
        0,                      // normalMapID if used
        torusModelData.vao,     // VAO from setupModelBuffers
        static_cast<GLsizei>(torusModelData.indices.size()), // Index count
        torusBoundingBox,       // bounding box
        glm::vec3(0.0f, 1.0f, 0.0f), //  rotation axis
        0.0f,                       //  rotation angle
        true                       // isStatic?
    );



    // add a std::vector of simple point lights from namespace utils_light
    std::vector<utils_light::SimplePointLight> simpleLights;

    // Add a simple point light to the scene
    int newLightID = utils_light::addLight(
        simpleLights,
        glm::vec3(5.0f, 4.0f, 9.0f), // position
        glm::vec3(1.0f, 0.0f, 0.0f), // color
        1.0f                         // intensity
    );

    // Add a simple point light to the scene 2
    int newLightID2 = utils_light::addLight(
        simpleLights,
        glm::vec3(5.0f, 4.0f, 5.0f), // position
        glm::vec3(0.0f, 0.0f, 1.0f), // color
        1.0f                         // intensity
    );

    // update a light position during the loop
    // utils_light::updateLightPosition(simpleLights, newLightID, glm::vec3(5.0f, 2.0f, 1.0f));

    // change colro and intensity of the light
    // utils_light::updateLightColor(simpleLights, newLightID, glm::vec3(0.2f, 1.0f, 0.2f));
    // utils_light::updateLightIntensity(simpleLights, newLightID, 3.5f);

    // remove the light from the scene
    // bool lightRemoved = utils_light::removeLight(simpleLights, newLightID);
    // if (lightRemoved) {
    //     std::cout << "Light with ID " << newLightID << " removed from the scene." << std::endl;
    // } else {
    //     std::cerr << "Failed to remove light with ID " << newLightID << " from the scene." << std::endl;
    // }


    // =======================
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Main loop variables
    bool done = false;
    std::cout << "Entering main loop" << std::endl;

    while (!done)
    {
        // Calculate delta time
        float currentFrame = windowManager.getTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float adjustedSpeed = cameraSpeed * deltaTime;

        // Update FPS counter
        frameCount++;
        fpsTimer += deltaTime;
        if (fpsTimer >= 1.0f)
        {
            fps = frameCount;
            frameCount = 0;
            fpsTimer -= 1.0f;
        }

        // Update window title with camera position every frame
        // std::string newTitle = "Boules - FPS: " + std::to_string(fps) + " - Position: (" + std::to_string(cameraPos.x) + ", " + std::to_string(cameraPos.z) + ")";
        std::string newTitle = std::to_string(cameraPos.x) + ", " + std::to_string(cameraPos.z);
        // std::string newTitle = "FPS: " + std::to_string(fps);
        SDL_SetWindowTitle(windowManager.getWindow(), newTitle.c_str());

        // Update light intensity dynamically within the loop
        glm::vec3 lightIntensity = glm::vec3(
            (sin(currentFrame) + 1.0f) / 2.0f,       // Red oscillates between 0 and 1
            (cos(currentFrame) + 1.0f) / 2.0f,       // Green oscillates between 0 and 1
            (sin(currentFrame * 0.5f) + 1.0f) / 2.0f // Blue oscillates more slowly between 0 and 1
        );

        // white light
        // glm::vec3 lightIntensity = glm::vec3(1.0f);

        // green light
        // glm::vec3 lightIntensity = glm::vec3(0.0f, 1.0f, 0.0f);

        // even handler 
        utils_game_loop::eventHandler(windowManager, done, isRockingChairPaused, rockingChairStartTime, rockingChairPausedTime, yaw, pitch, cameraFront, currentFrame);

        // Movement direction vectors projected onto the XZ-plane
        glm::vec3 frontDirection = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
        glm::vec3 rightDirection = glm::normalize(glm::cross(frontDirection, cameraUp));

        // Keyboard input for movement
        const Uint8 *state = SDL_GetKeyboardState(NULL);

        glm::vec3 proposedCameraPos = cameraPos; // Temporary camera position for collision testing

        // Calculate proposed camera position based on input
        if (state[SDL_SCANCODE_W])
        {
            proposedCameraPos += adjustedSpeed * frontDirection;
        }
        if (state[SDL_SCANCODE_S])
        {
            proposedCameraPos -= adjustedSpeed * frontDirection;
        }
        if (state[SDL_SCANCODE_A])
        {
            proposedCameraPos -= rightDirection * adjustedSpeed;
        }
        if (state[SDL_SCANCODE_D])
        {
            proposedCameraPos += rightDirection * adjustedSpeed;
        }

        // Check collision against all objects
        bool collisionDetected = false;
        for (const auto &object : utils_scene::sceneObjects)
        {
            if (checkCollision(proposedCameraPos, cameraRadius, cameraHeight, object.boundingBox))
            {
                collisionDetected = true;
                break; // Stop further checking if a collision is found
            }
        }

        // Update camera position only if no collision is detected
        if (!collisionDetected)
        {
            cameraPos = proposedCameraPos;
        }

        // Keep the camera at the initial y position
        cameraPos.y = cameraPosY;

        // Define MVP matrices
        glm::mat4 ProjMatrix = glm::perspective(
            glm::radians(70.0f),                 // Field of View
            window_width / (float)window_height, // Aspect ratio
            0.1f,                                // Near clipping plane
            100.0f                               // Far clipping plane
        );

        glm::mat4 ViewMatrix = glm::lookAt(
            cameraPos,               // Camera position
            cameraPos + cameraFront, // Look at target
            cameraUp                 // Up vector
        );

        glm::vec3 spiralCenter(9.0f, 1.0f, 3.0f); // Center of the spiral

        // Spiral movement around spiralCenter
        glm::vec3 lightPosWorld;
        lightPosWorld.x = spiralCenter.x + spiralRadius * cos(currentFrame * spiralSpeed);
        lightPosWorld.y = fixedHeight + spiralCenter.y;
        lightPosWorld.z = spiralCenter.z + spiralRadius * sin(currentFrame * spiralSpeed);

        // fixed light position
        // glm::vec3 lightPosWorld = glm::vec3(2.0f, 0.6f, 2.0f);

        // light position on the camera
        // glm::vec3 lightPosWorld = cameraPos + glm::vec3(0.0f, 1.0f, 0.0f); // Slightly elevate the light position above the camera

        // Transform light position to view space
        glm::vec3 lightPosViewSpace = glm::vec3(ViewMatrix * glm::vec4(lightPosWorld, 1.0f));

        // Recalculate light space matrix
        glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 50.0f);
        glm::mat4 lightView = glm::lookAt(lightPosWorld, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        // Create six view matrices for the cube map faces
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosWorld, lightPosWorld + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosWorld, lightPosWorld + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosWorld, lightPosWorld + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosWorld, lightPosWorld + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosWorld, lightPosWorld + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPosWorld, lightPosWorld + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

        // First Pass: Render scene from light's perspective to generate shadow map
        glViewport(0, 0, 4096, 4096); // Match shadow map resolution
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Use the depth shader program
        depthShader.use();
        glUniform1f(glGetUniformLocation(depthShader.getGLId(), "farPlane"), farPlane);
        glUniform3fv(glGetUniformLocation(depthShader.getGLId(), "lightPos"), 1, glm::value_ptr(lightPosWorld));

        // Rocking chair parameters
        double frequency = 0.30;           // Rocking frequency (cycles per second)
        double length = 0.08;    // Maximum angle in degrees
        double radius = 0.3;              // Radius of the rocking base

        // dynamic loop 
        utils_game_loop::dynamic_loop(deltaTime, lastFrame, currentFrame, windowManager, cameraPos, cameraFront, cameraUp, cameraSpeed, done, isRockingChairPaused, rockingChairStartTime, rockingChairPausedTime, yaw, pitch, radius, frequency, radius, length, cameraHeight);

        // First Pass: Render scene to depth cube map
        for (unsigned int i = 0; i < 6; ++i) {
            // Bind the framebuffer and attach the current cube map face
            glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, depthCubeMap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);

            // Set viewport and clear depth buffer
            glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
            glClear(GL_DEPTH_BUFFER_BIT);

            // Use the depth shader program
            depthShader.use();
            glUniform1f(glGetUniformLocation(depthShader.getGLId(), "farPlane"), farPlane);
            glUniform3fv(glGetUniformLocation(depthShader.getGLId(), "lightPos"), 1, glm::value_ptr(lightPosWorld));

            // Set the shadow matrix for the current face
            glUniformMatrix4fv(glGetUniformLocation(depthShader.getGLId(), "shadowMatrix"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));

            // Render scene objects
            for (const auto &object : utils_scene::sceneObjects) {
                glm::mat4 modelMatrix = glm::mat4(1.0f);
                modelMatrix = glm::translate(modelMatrix, object.position);
                if (object.rotationAngle != 0.0f) {
                    modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotationAngle), object.rotationAxis);
                }
                modelMatrix = glm::scale(modelMatrix, object.scale);

                // Set model matrix for depth shader
                glUniformMatrix4fv(glGetUniformLocation(depthShader.getGLId(), "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

                glBindVertexArray(object.vaoID);
                if (object.type == utils_scene::ObjectType::Cube) {
                    glDrawElements(GL_TRIANGLES, object.indexCount, GL_UNSIGNED_INT, 0);
                } else if (object.type == utils_scene::ObjectType::Sphere) {
                    glDrawArrays(GL_TRIANGLES, 0, object.indexCount);
                } else if (object.type == utils_scene::ObjectType::Model) {
                    glDrawElements(GL_TRIANGLES, object.indexCount, GL_UNSIGNED_INT, 0);
                }
                glBindVertexArray(0);
            }

            // Optional: Unbind the framebuffer after each face
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // Second Pass: Render the scene normally with point light
        glViewport(0, 0, window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // room1.use();

        // Set the shader program to use, room1 for x udner 20.5 and room2 for x over 20.5
        utils_loader::Shader* currentRoom = &room1;

        if (cameraPos.x < 20.5f) {
            currentRoom = &room1;
        } else {
            currentRoom = &room2;
        }

        currentRoom->use();

        // simpel point lights here
        int numLights = (int)simpleLights.size();
        if (numLights > MAX_ADDITIONAL_LIGHTS) {
            numLights = MAX_ADDITIONAL_LIGHTS;
        }

        GLint numLightsLoc = glGetUniformLocation(currentRoom->getGLId(), "uNumAdditionalLights");
        glUniform1i(numLightsLoc, numLights);

        // convert to view space
        std::vector<glm::vec3> additionalLightPosViewSpace;
        for (const auto& light : simpleLights) {
            glm::vec4 posView = ViewMatrix * glm::vec4(light.position, 1.0f);
            additionalLightPosViewSpace.emplace_back(glm::vec3(posView));
        }

        // For each light, pass position, color, intensity
        for (int i = 0; i < numLights; ++i) {
            std::string idx = std::to_string(i);

            // Position
            GLint posLoc = glGetUniformLocation(
                currentRoom->getGLId(),
                ("uAdditionalLightPos[" + idx + "]").c_str()
            );
            glUniform3fv(posLoc, 1, glm::value_ptr(additionalLightPosViewSpace[i]));

            // Color
            GLint colorLoc = glGetUniformLocation(
                currentRoom->getGLId(),
                ("uAdditionalLightColor[" + idx + "]").c_str()
            );
            glUniform3fv(colorLoc, 1, glm::value_ptr(simpleLights[i].color));

            // Intensity
            GLint intenLoc = glGetUniformLocation(
                currentRoom->getGLId(),
                ("uAdditionalLightIntensity[" + idx + "]").c_str()
            );
            glUniform1f(intenLoc, simpleLights[i].intensity);
        }

        // Set light properties
        glUniform3fv(uLightPos_vsLocation, 1, glm::value_ptr(lightPosViewSpace));
        glUniform3fv(uLightIntensityLocation, 1, glm::value_ptr(lightIntensity));

        // Set the updated light space matrix
        glUniformMatrix4fv(glGetUniformLocation(currentRoom->getGLId(), "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // Bind the depth cube map to texture unit 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
        glUniform1i(glGetUniformLocation(currentRoom->getGLId(), "depthMap"), 1);

        // Set light position in world space
        glUniform3fv(glGetUniformLocation(currentRoom->getGLId(), "lightPosWorld"), 1, glm::value_ptr(lightPosWorld));

        // Set camera position in world space
        glUniform3fv(glGetUniformLocation(currentRoom->getGLId(), "cameraPosWorld"), 1, glm::value_ptr(cameraPos));

        // Set far plane
        glUniform1f(glGetUniformLocation(currentRoom->getGLId(), "farPlane"), farPlane);

        // Render all scene objects
        for (const auto &object : utils_scene::sceneObjects)
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, object.position);
            if (object.rotationAngle != 0.0f)
            {
                modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotationAngle), object.rotationAxis);
            }
            modelMatrix = glm::scale(modelMatrix, object.scale);

            glm::mat4 mvMatrix = ViewMatrix * modelMatrix;
            glm::mat4 mvpMatrix = ProjMatrix * mvMatrix;
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix)));

            // Set uniforms for shaders
            glUniformMatrix4fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix)); // For vertex shader
            glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
            glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix3fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            // Set material properties
            glm::vec3 Kd = glm::vec3(0.6f); // Fixed diffuse color (e.g., gray)
            glm::vec3 Ks = glm::vec3(0.3f); // Fixed specular color
            float shininess = 32.0f;        // Moderate shininess
            glUniform3fv(uKdLocation, 1, glm::value_ptr(Kd));
            glUniform3fv(uKsLocation, 1, glm::value_ptr(Ks));
            glUniform1f(uShininessLocation, shininess);

            // Handle textures
            if (object.useTexture && object.textureID != 0)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, object.textureID);
                glUniform1i(uTextureLocation, 0); // Ensure the texture sampler uses the correct unit
                glUniform1f(uUseTextureLocation, 1.0f);
            }
            else
            {
                glUniform1f(uUseTextureLocation, 0.0f);
            }

            if (object.normalMapID != 0)
            {
                glActiveTexture(GL_TEXTURE2); // Bind to texture unit 2 (or another unused unit)
                glBindTexture(GL_TEXTURE_2D, object.normalMapID);
                glUniform1i(glGetUniformLocation(currentRoom->getGLId(), "uNormalMap"), 2);
                glUniform1f(glGetUniformLocation(currentRoom->getGLId(), "uUseNormalMap"), 1.0f);
            }
            else
            {
                glUniform1f(glGetUniformLocation(currentRoom->getGLId(), "uUseNormalMap"), 0.0f); // Disable normal map usage
            }

            glBindVertexArray(object.vaoID);
            if (object.type == utils_scene::ObjectType::Cube)
            {
                glDrawElements(GL_TRIANGLES, object.indexCount, GL_UNSIGNED_INT, 0);
            }
            else if (object.type == utils_scene::ObjectType::Sphere)
            {
                glDrawArrays(GL_TRIANGLES, 0, object.indexCount);
            }
            else if (object.type == utils_scene::ObjectType::Model)
            {
                glDrawElements(GL_TRIANGLES, object.indexCount, GL_UNSIGNED_INT, 0);
            }

            // Unbind textures if they are used
            if (object.useTexture && object.textureID != 0)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0); // Unbind base texture
            }
            if (object.normalMapID != 0)
            {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, 0); // Unbind normal map texture
            }
        }

        // Render a small sphere to represent the light source
        {
            // Create model matrix for the light sphere
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, lightPosWorld);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));

            // Calculate matrices
            glm::mat4 mvMatrix = ViewMatrix * modelMatrix;
            glm::mat4 mvpMatrix = ProjMatrix * mvMatrix;
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix)));

            // Use the unified shader program
            currentRoom->use();

            // **Set transformation matrices**
            glUniformMatrix4fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
            glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix3fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            // **Set camera position in world space (for lighting calculations)**
            glUniform3fv(glGetUniformLocation(currentRoom->getGLId(), "cameraPosWorld"), 1, glm::value_ptr(cameraPos));

            // **Set light position in world space (even if not needed for the light sphere)**
            glUniform3fv(glGetUniformLocation(currentRoom->getGLId(), "lightPosWorld"), 1, glm::value_ptr(lightPosWorld));

            // **Set light properties**
            glUniform3fv(uLightPos_vsLocation, 1, glm::value_ptr(lightPosViewSpace));
            glUniform3fv(uLightIntensityLocation, 1, glm::value_ptr(lightIntensity));

            // **Set far plane**
            glUniform1f(glGetUniformLocation(currentRoom->getGLId(), "farPlane"), farPlane);

            // Bind depth cube map
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
            glUniform1i(glGetUniformLocation(currentRoom->getGLId(), "depthMap"), 1);

            // Set material properties (emissive)
            glm::vec3 Kd = lightIntensity / 5.0f; // Adjustable
            glm::vec3 Ks = glm::vec3(0.0f);       // No specular for the light indicator
            float shininess = 1.0f;
            glUniform3fv(uKdLocation, 1, glm::value_ptr(Kd));
            glUniform3fv(uKsLocation, 1, glm::value_ptr(Ks));
            glUniform1f(uShininessLocation, shininess);

            // Disable textures
            glUniform1f(uUseTextureLocation, 0.0f);
            glUniform1f(glGetUniformLocation(currentRoom->getGLId(), "uUseNormalMap"), 0.0f);

            // Draw the sphere
            glBindVertexArray(sphereVAO);
            glDrawArrays(GL_TRIANGLES, 0, sphereVertexCountGL);
            glBindVertexArray(0);
        }

        // Swap buffers
        windowManager.swapBuffers();
    }

    // Clean up resources
    glDeleteBuffers(1, &sphereVBO);
    glDeleteVertexArrays(1, &sphereVAO);

    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);
    glDeleteVertexArrays(1, &cubeVAO);

    for (const auto &tex : textures)
    {
        glDeleteTextures(1, &tex.second);
    }

    // Clean up model buffers
    glDeleteBuffers(1, &heaterModelData.vbo);
    glDeleteBuffers(1, &heaterModelData.ebo);
    glDeleteVertexArrays(1, &heaterModelData.vao);

    std::cout << "Program terminated successfully" << std::endl;

    return 0;
}