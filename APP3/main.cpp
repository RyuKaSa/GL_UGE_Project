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
#include "utils/material.hpp"
#include "utils/material_manager.hpp"

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
        applicationPath.dirPath() + "APP3/shaders/room1.fs.glsl");

    // room 2 is to be acticaved when camera position is in the second room, i.e., x > 20.5, no condition on z or y
    utils_loader::Shader room2(
        applicationPath.dirPath() + "APP3/shaders/room2.vs.glsl",
        applicationPath.dirPath() + "APP3/shaders/room2.fs.glsl");

    utils_loader::Shader depthShader(
        applicationPath.dirPath() + "APP3/shaders/point_shadow_depth.vs.glsl",
        applicationPath.dirPath() + "APP3/shaders/point_shadow_depth.fs.glsl");

    // skybox shader
    utils_loader::Shader skyboxShader(
        applicationPath.dirPath() + "APP3/shaders/skybox.vs.glsl",
        applicationPath.dirPath() + "APP3/shaders/skybox.fs.glsl");

    // light shader
    utils_loader::Shader lightShader(
        applicationPath.dirPath() + "APP3/shaders/light.vs.glsl",
        applicationPath.dirPath() + "APP3/shaders/light.fs.glsl");

    // Check shaders
    if (room1.getID() == 0 || room2.getID() == 0 || depthShader.getID() == 0 || skyboxShader.getID() == 0)
    {
        std::cerr << "Failed to compile/link one or more shaders. Exiting." << std::endl;
        return -1;
    }

    room1.use();
    std::cout << "Unified shader program in use" << std::endl;

    // Load textures
    GLuint textureID, stoneTextureID, brownTerracottaTextureID, soccerTextureID;
    GLuint textureID_normalMap, stoneTextureID_normalMap, brownTerracottaTextureID_normalMap, soccerTextureID_normalMap;
    GLuint chairBaseColorTextureID, chairNormalMapTextureID;

    GLuint texture_ID_deepslate_emerald_ore, texture_ID_deepslate_emerald_ore_n, texture_ID_deepslate_emerald_ore_s;
    GLuint texture_ID_glass, texture_ID_purple_stained_glass;
    GLuint testure_ID_iron_block, texture_ID_iron_block_n, texture_ID_iron_block_s;
    GLuint texture_ID_note_block, texture_ID_note_block_n, texture_ID_note_block_s;
    GLuint texture_ID_oak_planks, texture_ID_oak_planks_n, texture_ID_oak_planks_s;
    GLuint texture_ID_stone_bricks, texture_ID_stone_bricks_n, texture_ID_stone_bricks_s;
    GLuint skyboxTextureID;
    GLuint sunTextureID, mercuryTextureID, venusTextureID, venus_atmosphereTextureID;
    GLuint marsTextureID, jupiterTextureID, saturnTextureID, saturn_ringTextureID;
    GLuint uranusTextureID, neptuneTextureID;
    GLuint earthTextureID, earth_nTextureID, earth_sTextureID, earth_atmosphereTextureID;

    utils_loader::loadTextures(textureID, stoneTextureID, brownTerracottaTextureID, soccerTextureID,
                               textureID_normalMap, stoneTextureID_normalMap, brownTerracottaTextureID_normalMap, soccerTextureID_normalMap,
                               chairBaseColorTextureID, chairNormalMapTextureID,
                               texture_ID_deepslate_emerald_ore, texture_ID_deepslate_emerald_ore_n, texture_ID_deepslate_emerald_ore_s,
                               texture_ID_glass, texture_ID_purple_stained_glass,
                               testure_ID_iron_block, texture_ID_iron_block_n, texture_ID_iron_block_s,
                               texture_ID_note_block, texture_ID_note_block_n, texture_ID_note_block_s,
                               texture_ID_oak_planks, texture_ID_oak_planks_n, texture_ID_oak_planks_s,
                               texture_ID_stone_bricks, texture_ID_stone_bricks_n, texture_ID_stone_bricks_s,
                               skyboxTextureID,
                               sunTextureID, mercuryTextureID, venusTextureID, venus_atmosphereTextureID,
                               marsTextureID, jupiterTextureID, saturnTextureID, saturn_ringTextureID,
                               uranusTextureID, neptuneTextureID,
                               earthTextureID, earth_nTextureID, earth_sTextureID, earth_atmosphereTextureID,
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

    room1.use(); // Activate room1 shader program

    // Assign sampler uniforms to texture units
    glUniform1i(glGetUniformLocation(room1.getID(), "uTexture"), 0);
    glUniform1i(glGetUniformLocation(room1.getID(), "uSpecularMap"), 3);
    glUniform1i(glGetUniformLocation(room1.getID(), "uNormalMap"), 2);
    glUniform1i(glGetUniformLocation(room1.getID(), "depthMap"), 1);

    room1.use(); // Unbind the shader program

    // Repeat for room2 shader if necessary
    room2.use();
    glUniform1i(glGetUniformLocation(room2.getID(), "uTexture"), 0);
    glUniform1i(glGetUniformLocation(room2.getID(), "uSpecularMap"), 3);
    glUniform1i(glGetUniformLocation(room2.getID(), "uNormalMap"), 2);
    glUniform1i(glGetUniformLocation(room2.getID(), "depthMap"), 1);
    room2.use(); // Unbind the shader program

    // Set up skybox shader
    skyboxShader.use();
    std::cout << "Sky Shader program in use" << std::endl;

    // Get uniform locations
    GLint sky_uMVPMatrixLocation = glGetUniformLocation(skyboxShader.getGLId(), "uMVPMatrix");
    GLint sky_uMVMatrixLocation = glGetUniformLocation(skyboxShader.getGLId(), "uMVMatrix");
    GLint sky_uNormalMatrixLocation = glGetUniformLocation(skyboxShader.getGLId(), "uNormalMatrix");
    GLint sky_uTextureLocation = glGetUniformLocation(skyboxShader.getGLId(), "uTexture");
    GLint sky_uUseTextureLocation = glGetUniformLocation(skyboxShader.getGLId(), "uUseTexture");

    GLint sky_uKdLocation = glGetUniformLocation(skyboxShader.getGLId(), "uKd");
    GLint sky_uKsLocation = glGetUniformLocation(skyboxShader.getGLId(), "uKs");
    GLint sky_uShininessLocation = glGetUniformLocation(skyboxShader.getGLId(), "uShininess");
    GLint sky_uLightPos_vsLocation = glGetUniformLocation(skyboxShader.getGLId(), "uLightPos_vs");
    GLint sky_uLightIntensityLocation = glGetUniformLocation(skyboxShader.getGLId(), "uLightIntensity");

    // sample skybox texture
    glUniform1i(glGetUniformLocation(skyboxShader.getID(), "uTexture"), 0);
    glUniform1i(glGetUniformLocation(skyboxShader.getID(), "uSpecularMap"), 3);
    glUniform1i(glGetUniformLocation(skyboxShader.getID(), "uNormalMap"), 2);
    glUniform1i(glGetUniformLocation(skyboxShader.getID(), "depthMap"), 1);

    // unbind the shader program
    skyboxShader.use();

    // Set up light shader
    lightShader.use();
    std::cout << "Light Shader program in use" << std::endl;

    // Get uniform locations
    GLint light_uMVPMatrixLocation = glGetUniformLocation(lightShader.getGLId(), "uMVPMatrix");
    GLint light_uMVMatrixLocation = glGetUniformLocation(lightShader.getGLId(), "uMVMatrix");
    GLint light_uNormalMatrixLocation = glGetUniformLocation(lightShader.getGLId(), "uNormalMatrix");
    GLint light_uTextureLocation = glGetUniformLocation(lightShader.getGLId(), "uTexture");
    GLint light_uUseTextureLocation = glGetUniformLocation(lightShader.getGLId(), "uUseTexture");

    GLint light_uKdLocation = glGetUniformLocation(lightShader.getGLId(), "uKd");
    GLint light_uKsLocation = glGetUniformLocation(lightShader.getGLId(), "uKs");
    GLint light_uShininessLocation = glGetUniformLocation(lightShader.getGLId(), "uShininess");
    GLint light_uLightPos_vsLocation = glGetUniformLocation(lightShader.getGLId(), "uLightPos_vs");
    GLint light_uLightIntensityLocation = glGetUniformLocation(lightShader.getGLId(), "uLightIntensity");

    // sample light texture
    glUniform1i(glGetUniformLocation(lightShader.getID(), "uTexture"), 0);
    glUniform1i(glGetUniformLocation(lightShader.getID(), "uSpecularMap"), 3);
    glUniform1i(glGetUniformLocation(lightShader.getID(), "uNormalMap"), 2);
    glUniform1i(glGetUniformLocation(lightShader.getID(), "depthMap"), 1);

    // unbind the shader program
    lightShader.use();

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    std::cout << "Depth test enabled" << std::endl;

    // Set the clear color to a dark gray
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Light space matrix for shadows
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.0f);
    glm::mat4 lightView = glm::lookAt(glm::vec3(5.0f, 10.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    // Setup projection matrix for cube map
    float nearPlane = 1.0f;
    float farPlane = 50.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);

    // Camera parameters
    glm::vec3 cameraPos(1.0f, 2.0f, 1.0f);    // Initial position of the camera
    glm::vec3 cameraFront(0.0f, 0.0f, -1.0f); // Direction the camera is looking at
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);     // Up vector

    float cameraPosY = cameraPos.y; // Store the initial y position

    float yaw = -90.0f; // Yaw angle initialized to -90 degrees to look towards negative Z
    float pitch = 0.0f; // Pitch angle

    float cameraSpeed = 7.0f; // Adjust accordingly (10.0f is kinda fast but good for debugging)
    float deltaTime = 0.0f;    // Time between current frame and last frame
    float lastFrame = 0.0f;    // Time of last frame

    // Enable relative mouse mode to capture mouse movement
    SDL_SetRelativeMouseMode(SDL_TRUE);

    int frameCount = 0;
    float fpsTimer = 0.0f;
    int fps = 0;

    // ====================
    // create materials for the scene

    // Define the alpha value for transparency
    float alphaOpaque = 1.0f; // Fully opaque
    float alphaTransparent1 = 0.1f;
    float alphaTransparent2 = 0.2f;
    float alphaTransparent3 = 0.3f;
    float alphaTransparent4 = 0.4f;
    float alphaTransparent5 = 0.5f;
    float alphaTransparent6 = 0.6f;
    float alphaTransparent7 = 0.7f;
    float alphaTransparent8 = 0.8f;
    float alphaTransparent9 = 0.9f;

    // Material for the floor and walls using stone texture
    Material stoneMaterial;
    stoneMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color (will be tinted by texture)
    stoneMaterial.hasDiffuseMap = true;
    stoneMaterial.diffuseMapID = stoneTextureID;
    stoneMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    stoneMaterial.shininess = 32.0f;                // Shininess exponent
    stoneMaterial.hasSpecularMap = false;
    stoneMaterial.specularMapID = 0;
    stoneMaterial.hasNormalMap = true;
    stoneMaterial.normalMapID = stoneTextureID_normalMap;
    stoneMaterial.alpha = alphaOpaque; // Opaque

    // Material for walls without stone texture but with general texture
    Material wallMaterial;
    wallMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    wallMaterial.hasDiffuseMap = true;
    wallMaterial.diffuseMapID = textureID;
    wallMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    wallMaterial.shininess = 32.0f;                // Shininess exponent
    wallMaterial.hasSpecularMap = false;
    wallMaterial.specularMapID = 0;
    wallMaterial.hasNormalMap = true;
    wallMaterial.normalMapID = textureID_normalMap;
    wallMaterial.alpha = alphaOpaque; // Opaque

    // Material for separation walls with brown terracotta texture
    Material terracottaMaterial;
    terracottaMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    terracottaMaterial.hasDiffuseMap = true;
    terracottaMaterial.diffuseMapID = brownTerracottaTextureID;
    terracottaMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    terracottaMaterial.shininess = 32.0f;                // Shininess exponent
    terracottaMaterial.hasSpecularMap = false;
    terracottaMaterial.specularMapID = 0;
    terracottaMaterial.hasNormalMap = true;
    terracottaMaterial.normalMapID = brownTerracottaTextureID_normalMap;
    terracottaMaterial.alpha = alphaOpaque; // Opaque

    // Material for transparent cubes without textures
    Material transparentMaterialNoTexture;
    transparentMaterialNoTexture.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    transparentMaterialNoTexture.hasDiffuseMap = false;
    transparentMaterialNoTexture.diffuseMapID = 0;
    transparentMaterialNoTexture.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    transparentMaterialNoTexture.shininess = 32.0f;                // Shininess exponent
    transparentMaterialNoTexture.hasSpecularMap = false;
    transparentMaterialNoTexture.specularMapID = 0;
    transparentMaterialNoTexture.hasNormalMap = false;
    transparentMaterialNoTexture.normalMapID = 0;
    transparentMaterialNoTexture.alpha = alphaTransparent1; // 30% opaque

    // Material for transparent cubes with textures
    Material transparentMaterialWithTexture;
    transparentMaterialWithTexture.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    transparentMaterialWithTexture.hasDiffuseMap = true;
    transparentMaterialWithTexture.diffuseMapID = textureID;         // Ensure textureID is loaded
    transparentMaterialWithTexture.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    transparentMaterialWithTexture.shininess = 32.0f;                // Shininess exponent
    transparentMaterialWithTexture.hasSpecularMap = false;           // Assuming no specular map
    transparentMaterialWithTexture.specularMapID = 0;
    transparentMaterialWithTexture.hasNormalMap = true;
    transparentMaterialWithTexture.normalMapID = textureID_normalMap; // Ensure textureID_normalMap is loaded
    transparentMaterialWithTexture.alpha = alphaTransparent2;         // 50% opaque

    // // Material for the heater model (no textures)
    // Material heaterMaterial;
    // heaterMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    // heaterMaterial.hasDiffuseMap = false;
    // heaterMaterial.diffuseMapID = 0;
    // heaterMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    // heaterMaterial.shininess = 32.0f;                // Shininess exponent
    // heaterMaterial.hasSpecularMap = false;
    // heaterMaterial.specularMapID = 0;
    // heaterMaterial.hasNormalMap = false;
    // heaterMaterial.normalMapID = 0;
    // heaterMaterial.alpha = alphaOpaque; // Opaque

    // Material for the rocking chair model with textures
    Material rockingChairMaterial;
    rockingChairMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    rockingChairMaterial.hasDiffuseMap = true;
    rockingChairMaterial.diffuseMapID = chairBaseColorTextureID;
    rockingChairMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    rockingChairMaterial.shininess = 32.0f;                // Shininess exponent
    rockingChairMaterial.hasSpecularMap = false;           // Assuming no specular map
    rockingChairMaterial.specularMapID = 0;
    rockingChairMaterial.hasNormalMap = true;
    rockingChairMaterial.normalMapID = chairNormalMapTextureID;
    rockingChairMaterial.alpha = alphaOpaque; // Opaque

    // Material for the torus model (no textures)
    Material torusMaterial;
    torusMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    torusMaterial.hasDiffuseMap = false;
    torusMaterial.diffuseMapID = 0;
    torusMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    torusMaterial.shininess = 32.0f;                // Shininess exponent
    torusMaterial.hasSpecularMap = false;
    torusMaterial.specularMapID = 0;
    torusMaterial.hasNormalMap = false;
    torusMaterial.normalMapID = 0;
    torusMaterial.alpha = alphaOpaque; // Opaque

    // Define the soccer ball material
    Material soccerMaterial;
    soccerMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    soccerMaterial.hasDiffuseMap = true;
    soccerMaterial.diffuseMapID = soccerTextureID;   // Ensure soccerTextureID is loaded
    soccerMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    soccerMaterial.shininess = 32.0f;                // Shininess exponent
    soccerMaterial.hasSpecularMap = false;           // Assuming no specular map
    soccerMaterial.specularMapID = 0;
    soccerMaterial.hasNormalMap = true;
    soccerMaterial.normalMapID = soccerTextureID_normalMap; // Ensure soccerTextureID_normalMap is loaded
    soccerMaterial.alpha = alphaOpaque;

    // Define a reference to the MaterialManager singleton
    MaterialManager &materialManager = MaterialManager::getInstance();

    // Add materials to the MaterialManager function is addOrGetMaterial
    materialManager.addOrGetMaterial(stoneMaterial);
    materialManager.addOrGetMaterial(wallMaterial);
    materialManager.addOrGetMaterial(terracottaMaterial);
    materialManager.addOrGetMaterial(transparentMaterialNoTexture);
    materialManager.addOrGetMaterial(transparentMaterialWithTexture);
    // materialManager.addOrGetMaterial(heaterMaterial);
    materialManager.addOrGetMaterial(rockingChairMaterial);
    materialManager.addOrGetMaterial(torusMaterial);
    materialManager.addOrGetMaterial(soccerMaterial);

    // second set of materials, with specular maps
    // Material for stone_bricks
    Material stone_bricks_material;
    stone_bricks_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    stone_bricks_material.hasDiffuseMap = true;
    stone_bricks_material.diffuseMapID = texture_ID_stone_bricks;
    stone_bricks_material.Ks = glm::vec3(0.4f, 0.4f, 0.4f);
    stone_bricks_material.shininess = 16.0f;
    stone_bricks_material.hasSpecularMap = true;
    stone_bricks_material.specularMapID = texture_ID_stone_bricks_s;
    stone_bricks_material.hasNormalMap = true;
    stone_bricks_material.normalMapID = texture_ID_stone_bricks_n;
    stone_bricks_material.alpha = alphaOpaque;

    // // Material for transparent cubes with textures
    // Material transparentMaterialWithTexture;
    // transparentMaterialWithTexture.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    // transparentMaterialWithTexture.hasDiffuseMap = true;
    // transparentMaterialWithTexture.diffuseMapID = textureID; // Ensure textureID is loaded
    // transparentMaterialWithTexture.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    // transparentMaterialWithTexture.shininess = 32.0f; // Shininess exponent
    // transparentMaterialWithTexture.hasSpecularMap = false; // Assuming no specular map
    // transparentMaterialWithTexture.specularMapID = 0;
    // transparentMaterialWithTexture.hasNormalMap = true;
    // transparentMaterialWithTexture.normalMapID = textureID_normalMap; // Ensure textureID_normalMap is loaded
    // transparentMaterialWithTexture.alpha = alphaTransparent2; // 50% opaque

    // Material for glass and purple stained glass
    Material glass_material;
    glass_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    glass_material.hasDiffuseMap = true;
    glass_material.diffuseMapID = texture_ID_glass;
    glass_material.Ks = glm::vec3(0.9f, 0.9f, 0.9f);
    glass_material.shininess = 130.0f;
    glass_material.hasSpecularMap = false;
    glass_material.specularMapID = 0;
    glass_material.hasNormalMap = false;
    glass_material.normalMapID = 0;
    glass_material.alpha = alphaTransparent5;

    Material purple_stained_glass_material;
    purple_stained_glass_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    purple_stained_glass_material.hasDiffuseMap = true;
    purple_stained_glass_material.diffuseMapID = texture_ID_purple_stained_glass;
    purple_stained_glass_material.Ks = glm::vec3(0.9f, 0.7f, 0.9f);
    purple_stained_glass_material.shininess = 130.0f;
    purple_stained_glass_material.hasSpecularMap = false;
    purple_stained_glass_material.specularMapID = 0;
    purple_stained_glass_material.hasNormalMap = false;
    purple_stained_glass_material.normalMapID = 0;
    purple_stained_glass_material.alpha = alphaTransparent5;

    // Material for iron block
    Material iron_block_material;
    iron_block_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    iron_block_material.hasDiffuseMap = true;
    iron_block_material.diffuseMapID = testure_ID_iron_block;
    iron_block_material.Ks = glm::vec3(0.8f, 0.8f, 0.8f);
    iron_block_material.shininess = 64.0f;
    iron_block_material.hasSpecularMap = true;
    iron_block_material.specularMapID = texture_ID_iron_block_s;
    iron_block_material.hasNormalMap = true;
    iron_block_material.normalMapID = texture_ID_iron_block_n;
    iron_block_material.alpha = alphaOpaque;

    // Material for note block
    Material note_block_material;
    note_block_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    note_block_material.hasDiffuseMap = true;
    note_block_material.diffuseMapID = texture_ID_note_block;
    note_block_material.Ks = glm::vec3(0.2f, 0.2f, 0.2f);
    note_block_material.shininess = 8.0f;
    note_block_material.hasSpecularMap = true;
    note_block_material.specularMapID = texture_ID_note_block_s;
    note_block_material.hasNormalMap = true;
    note_block_material.normalMapID = texture_ID_note_block_n;
    note_block_material.alpha = alphaOpaque;

    // Material for oak planks
    Material oak_planks_material;
    oak_planks_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    oak_planks_material.hasDiffuseMap = true;
    oak_planks_material.diffuseMapID = texture_ID_oak_planks;
    oak_planks_material.Ks = glm::vec3(0.3f, 0.3f, 0.3f);
    oak_planks_material.shininess = 12.0f;
    oak_planks_material.hasSpecularMap = true;
    oak_planks_material.specularMapID = texture_ID_oak_planks_s;
    oak_planks_material.hasNormalMap = true;
    oak_planks_material.normalMapID = texture_ID_oak_planks_n;
    oak_planks_material.alpha = alphaOpaque;

    // material deepslate_emerald_ore
    Material deepslate_emerald_ore_material;
    deepslate_emerald_ore_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    deepslate_emerald_ore_material.hasDiffuseMap = true;
    deepslate_emerald_ore_material.diffuseMapID = texture_ID_deepslate_emerald_ore;
    deepslate_emerald_ore_material.Ks = glm::vec3(0.6f, 0.8f, 0.6f);
    deepslate_emerald_ore_material.shininess = 54.0f;
    deepslate_emerald_ore_material.hasSpecularMap = true;
    deepslate_emerald_ore_material.specularMapID = texture_ID_deepslate_emerald_ore_s;
    deepslate_emerald_ore_material.hasNormalMap = true;
    deepslate_emerald_ore_material.normalMapID = texture_ID_deepslate_emerald_ore_n;
    deepslate_emerald_ore_material.alpha = alphaOpaque;

    // add transparent iron block
    Material transparent_iron_block_material;
    transparent_iron_block_material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);
    transparent_iron_block_material.hasDiffuseMap = true;
    transparent_iron_block_material.diffuseMapID = testure_ID_iron_block;
    transparent_iron_block_material.Ks = glm::vec3(0.8f, 0.8f, 0.8f);
    transparent_iron_block_material.shininess = 64.0f;
    transparent_iron_block_material.hasSpecularMap = true;
    transparent_iron_block_material.specularMapID = texture_ID_iron_block_s;
    transparent_iron_block_material.hasNormalMap = true;
    transparent_iron_block_material.normalMapID = texture_ID_iron_block_n;
    transparent_iron_block_material.alpha = alphaTransparent5;

    // Add materials to the MaterialManager
    materialManager.addOrGetMaterial(stone_bricks_material);
    materialManager.addOrGetMaterial(glass_material);
    materialManager.addOrGetMaterial(purple_stained_glass_material);
    materialManager.addOrGetMaterial(iron_block_material);
    materialManager.addOrGetMaterial(note_block_material);
    materialManager.addOrGetMaterial(oak_planks_material);
    materialManager.addOrGetMaterial(deepslate_emerald_ore_material);

    // sky material (big sphere around the scene)
    Material skyMaterial;
    skyMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    skyMaterial.hasDiffuseMap = true;
    skyMaterial.diffuseMapID = skyboxTextureID;
    skyMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    skyMaterial.shininess = 0.0f;                 // Shininess exponent
    skyMaterial.hasSpecularMap = false;           // Assuming no specular map
    skyMaterial.specularMapID = 0;
    skyMaterial.hasNormalMap = false;
    skyMaterial.normalMapID = 0;
    skyMaterial.alpha = alphaOpaque; // Opaque

    // light material
    Material lightMaterial;
    lightMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // White color
    lightMaterial.hasDiffuseMap = false;
    lightMaterial.diffuseMapID = 0;
    lightMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    lightMaterial.shininess = 32.0f;                // Shininess exponent
    lightMaterial.hasSpecularMap = false;
    lightMaterial.specularMapID = 0;
    lightMaterial.hasNormalMap = false;
    lightMaterial.normalMapID = 0;
    lightMaterial.alpha = alphaOpaque; // Opaque

    Material simpleLightMaterial;
    simpleLightMaterial.Kd = glm::vec3(1.0f, 1.0f, 1.0f); // Default white color
    simpleLightMaterial.Ks = glm::vec3(0.0f);            // No specular reflection
    simpleLightMaterial.shininess = 1.0f;               // Low shininess
    simpleLightMaterial.alpha = 1.0f;                   // Opaque

    // Add sky material to the MaterialManager
    materialManager.addOrGetMaterial(skyMaterial);
    materialManager.addOrGetMaterial(lightMaterial);
    materialManager.addOrGetMaterial(simpleLightMaterial);

    // planet materials in order

    // sun material
    Material sunMaterial;
    sunMaterial.Kd = glm::vec3(1.0f, 0.8f, 0.6f);
    sunMaterial.hasDiffuseMap = true;
    sunMaterial.diffuseMapID = sunTextureID;
    sunMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    sunMaterial.shininess = 0.0f;                // Shininess exponent
    sunMaterial.hasSpecularMap = false;
    sunMaterial.specularMapID = 0;
    sunMaterial.hasNormalMap = false;
    sunMaterial.normalMapID = 0;
    sunMaterial.alpha = alphaTransparent9; // Transparent

    // mercury material
    Material mercuryMaterial;
    mercuryMaterial.Kd = glm::vec3(0.5f, 0.5f, 0.5f);
    mercuryMaterial.hasDiffuseMap = true;
    mercuryMaterial.diffuseMapID = mercuryTextureID;
    mercuryMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    mercuryMaterial.shininess = 20.0f;                // Shininess exponent
    mercuryMaterial.hasSpecularMap = false;
    mercuryMaterial.specularMapID = 0;
    mercuryMaterial.hasNormalMap = false;
    mercuryMaterial.normalMapID = 0;
    mercuryMaterial.alpha = alphaOpaque; // Opaque

    // venus material
    Material venusMaterial;
    venusMaterial.Kd = glm::vec3(0.8f, 0.7f, 0.6f);
    venusMaterial.hasDiffuseMap = true;
    venusMaterial.diffuseMapID = venusTextureID;
    venusMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    venusMaterial.shininess = 25.0f;                // Shininess exponent
    venusMaterial.hasSpecularMap = false;
    venusMaterial.specularMapID = 0;
    venusMaterial.hasNormalMap = false;
    venusMaterial.normalMapID = 0;
    venusMaterial.alpha = alphaOpaque; // Opaque

    // venus atmosphere is transparent
    Material venusAtmosphereMaterial;
    venusAtmosphereMaterial.Kd = glm::vec3(0.7f, 0.7f, 0.9f);
    venusAtmosphereMaterial.hasDiffuseMap = true;
    venusAtmosphereMaterial.diffuseMapID = venus_atmosphereTextureID;
    venusAtmosphereMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    venusAtmosphereMaterial.shininess = 5.0f;                // Shininess exponent
    venusAtmosphereMaterial.hasSpecularMap = false;
    venusAtmosphereMaterial.specularMapID = 0;
    venusAtmosphereMaterial.hasNormalMap = false;
    venusAtmosphereMaterial.normalMapID = 0;
    venusAtmosphereMaterial.alpha = alphaTransparent3; // Transparent

    // earth material, has diffuse, specular, and normal maps
    Material earthMaterial;
    earthMaterial.Kd = glm::vec3(0.4f, 0.6f, 0.8f);
    earthMaterial.hasDiffuseMap = true;
    earthMaterial.diffuseMapID = earthTextureID;
    earthMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    earthMaterial.shininess = 30.0f;                // Shininess exponent
    earthMaterial.hasSpecularMap = true;
    earthMaterial.specularMapID = earth_sTextureID;
    earthMaterial.hasNormalMap = true;
    earthMaterial.normalMapID = earth_nTextureID;
    earthMaterial.alpha = alphaOpaque; // Opaque

    // earth atmosphere is transparent
    Material earthAtmosphereMaterial;
    earthAtmosphereMaterial.Kd = glm::vec3(0.6f, 0.7f, 0.9f); // White color
    earthAtmosphereMaterial.hasDiffuseMap = true;
    earthAtmosphereMaterial.diffuseMapID = earth_atmosphereTextureID;
    earthAtmosphereMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    earthAtmosphereMaterial.shininess = 8.0f;                // Shininess exponent
    earthAtmosphereMaterial.hasSpecularMap = false;
    earthAtmosphereMaterial.specularMapID = 0;
    earthAtmosphereMaterial.hasNormalMap = false;
    earthAtmosphereMaterial.normalMapID = 0;
    earthAtmosphereMaterial.alpha = alphaTransparent3;

    // mars material
    Material marsMaterial;
    marsMaterial.Kd = glm::vec3(0.6f, 0.3f, 0.2f);
    marsMaterial.hasDiffuseMap = true;
    marsMaterial.diffuseMapID = marsTextureID;
    marsMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    marsMaterial.shininess = 15.0f;                // Shininess exponent
    marsMaterial.hasSpecularMap = false;
    marsMaterial.specularMapID = 0;
    marsMaterial.hasNormalMap = false;
    marsMaterial.normalMapID = 0;
    marsMaterial.alpha = alphaOpaque; // Opaque

    // jupiter material
    Material jupiterMaterial;
    jupiterMaterial.Kd = glm::vec3(0.7f, 0.6f, 0.5f);
    jupiterMaterial.hasDiffuseMap = true;
    jupiterMaterial.diffuseMapID = jupiterTextureID;
    jupiterMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    jupiterMaterial.shininess = 18.0f;                // Shininess exponent
    jupiterMaterial.hasSpecularMap = false;
    jupiterMaterial.specularMapID = 0;
    jupiterMaterial.hasNormalMap = false;
    jupiterMaterial.normalMapID = 0;
    jupiterMaterial.alpha = alphaOpaque; // Opaque

    // saturn material
    Material saturnMaterial;
    saturnMaterial.Kd = glm::vec3(0.8f, 0.7f, 0.6f);
    saturnMaterial.hasDiffuseMap = true;
    saturnMaterial.diffuseMapID = saturnTextureID;
    saturnMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    saturnMaterial.shininess = 25.0f;                // Shininess exponent
    saturnMaterial.hasSpecularMap = false;
    saturnMaterial.specularMapID = 0;
    saturnMaterial.hasNormalMap = false;
    saturnMaterial.normalMapID = 0;
    saturnMaterial.alpha = alphaOpaque; // Opaque

    // saturn ring material
    Material saturnRingMaterial;
    saturnRingMaterial.Kd = glm::vec3(0.9f, 0.8f, 0.7f);
    saturnRingMaterial.hasDiffuseMap = true;
    saturnRingMaterial.diffuseMapID = saturn_ringTextureID;
    saturnRingMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    saturnRingMaterial.shininess = 60.0f;                // Shininess exponent
    saturnRingMaterial.hasSpecularMap = false;
    saturnRingMaterial.specularMapID = 0;
    saturnRingMaterial.hasNormalMap = false;
    saturnRingMaterial.normalMapID = 0;
    saturnRingMaterial.alpha = alphaTransparent4; // Transparent

    // uranus material
    Material uranusMaterial;
    uranusMaterial.Kd = glm::vec3(0.5f, 0.7f, 0.8f);
    uranusMaterial.hasDiffuseMap = true;
    uranusMaterial.diffuseMapID = uranusTextureID;
    uranusMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    uranusMaterial.shininess = 20.0f;                // Shininess exponent
    uranusMaterial.hasSpecularMap = false;
    uranusMaterial.specularMapID = 0;
    uranusMaterial.hasNormalMap = false;
    uranusMaterial.normalMapID = 0;
    uranusMaterial.alpha = alphaOpaque; // Opaque

    // neptune material
    Material neptuneMaterial;
    neptuneMaterial.Kd = glm::vec3(0.4f, 0.6f, 0.9f);
    neptuneMaterial.hasDiffuseMap = true;
    neptuneMaterial.diffuseMapID = neptuneTextureID;
    neptuneMaterial.Ks = glm::vec3(0.3f, 0.3f, 0.3f); // Specular color
    neptuneMaterial.shininess = 22.0f;                // Shininess exponent
    neptuneMaterial.hasSpecularMap = false;
    neptuneMaterial.specularMapID = 0;
    neptuneMaterial.hasNormalMap = false;
    neptuneMaterial.normalMapID = 0;
    neptuneMaterial.alpha = alphaOpaque; // Opaque

    // fuck pluto, not a real planet

    // add to manager
    materialManager.addOrGetMaterial(sunMaterial);
    materialManager.addOrGetMaterial(mercuryMaterial);
    materialManager.addOrGetMaterial(venusMaterial);
    materialManager.addOrGetMaterial(venusAtmosphereMaterial);
    materialManager.addOrGetMaterial(earthMaterial);
    materialManager.addOrGetMaterial(earthAtmosphereMaterial);
    materialManager.addOrGetMaterial(marsMaterial);
    materialManager.addOrGetMaterial(jupiterMaterial);
    materialManager.addOrGetMaterial(saturnMaterial);
    materialManager.addOrGetMaterial(saturnRingMaterial);
    materialManager.addOrGetMaterial(uranusMaterial);
    materialManager.addOrGetMaterial(neptuneMaterial);


    std::cout << "Materials created" << std::endl;
    // After loading textures
    for (const auto &material : materialManager.materials)
    {
        std::cout << "Material info " << std::endl;
        std::cout << "  Material Index: " << (&material - &materialManager.materials[0]) << std::endl;
        std::cout << "  Diffuse Map ID: " << material.diffuseMapID << std::endl;
        std::cout << "  Normal Map ID: " << material.normalMapID << std::endl;
    }
    // print size of materials
    std::cout << "Size of materials: " << materialManager.materials.size() << std::endl;

    // =======================
    // Scene objects creation

    // Prepare to add objects to the scene
    GLsizei cubeIndexCount = static_cast<GLsizei>(cubeIndices.size());
    // GLsizei sphereVertexCount = sphere.getVertexCount();

    // void addCube(const std::string &name,
    //             const glm::vec3 &position,
    //             const glm::vec3 &scale,
    //             const Material &material,
    //             const glm::vec3 &rotationAxis,
    //             float rotationAngle,
    //             GLuint vaoID,
    //             GLsizei indexCount,
    //             bool isStatic);

    // new materials, with specular maps, one of each
    glm::vec3 initialPosition(25.0f, 2.0f, 3.0f);
    glm::vec3 initialPosition2(25.0f, 2.0f, 4.0f);
    glm::vec3 initialSize(1.0f, 1.0f, 1.0f);
    // Adding "stone_bricks" cube
    utils_scene::addCube(
        "stone_bricks",              // Name
        initialPosition,             // Position
        initialSize,                 // Size
        stone_bricks_material,       // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    // Adding "glass" cube
    utils_scene::addTransparentCube(
        "glass",                     // Name
        initialPosition2,            // Position
        initialSize * 0.999f,        // Size (slightly smaller to avoid z-fighting)
        glass_material,              // Material
        glm::vec3(0.0f, 1.0f, 0.0f), // Rotation axis (Y-axis)
        0.0f,                        // Rotation angle (e.g., 0 degrees)
        cubeVAO,                     // VAO ID
        cubeIndexCount,              // Index count
        true                         // Is static
    );

    // Adding "purple_stained_glass" cube
    utils_scene::addTransparentCube(
        "purple_stained_glass",        // Name
        glm::vec3(25.0f, 2.0f, 5.0f),  // Position
        initialSize * 0.999f,          // Size (slightly smaller to avoid z-fighting)
        purple_stained_glass_material, // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle (e.g., 0 degrees)
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // Adding "iron_block" cube
    utils_scene::addCube(
        "iron_block",                 // Name
        glm::vec3(25.0f, 2.0f, 6.0f), // Position
        initialSize,                  // Size
        iron_block_material,          // Material
        glm::vec3(0.0f, 1.0f, 0.0f),  // Rotation axis (Y-axis)
        0.0f,                         // Rotation angle (e.g., 0 degrees)
        cubeVAO,                      // VAO ID
        cubeIndexCount,               // Index count
        true                          // Is static
    );

    // Adding "note_block" cube
    utils_scene::addCube(
        "note_block",                 // Name
        glm::vec3(25.0f, 2.0f, 7.0f), // Position
        initialSize,                  // Size
        note_block_material,          // Material
        glm::vec3(0.0f, 1.0f, 0.0f),  // Rotation axis (Y-axis)
        0.0f,                         // Rotation angle (e.g., 0 degrees)
        cubeVAO,                      // VAO ID
        cubeIndexCount,               // Index count
        true                          // Is static
    );

    // Adding "oak_planks" cube
    utils_scene::addCube(
        "oak_planks",                 // Name
        glm::vec3(25.0f, 2.0f, 8.0f), // Position
        initialSize,                  // Size
        oak_planks_material,          // Material
        glm::vec3(0.0f, 1.0f, 0.0f),  // Rotation axis (Y-axis)
        0.0f,                         // Rotation angle (e.g., 0 degrees)
        cubeVAO,                      // VAO ID
        cubeIndexCount,               // Index count
        true                          // Is static
    );

    // Adding "deepslate_emerald_ore" cube
    utils_scene::addCube(
        "deepslate_emerald_ore",        // Name
        glm::vec3(25.0f, 2.0f, 9.0f),   // Position
        initialSize,                    // Size
        deepslate_emerald_ore_material, // Material
        glm::vec3(0.0f, 1.0f, 0.0f),    // Rotation axis (Y-axis)
        0.0f,                           // Rotation angle (e.g., 0 degrees)
        cubeVAO,                        // VAO ID
        cubeIndexCount,                 // Index count
        true                            // Is static
    );

    // floor
    glm::vec3 origin(0.0f, 0.0f, 0.0f);
    glm::vec3 floorSize(42.0f, 1.0f, 24.0f);
    utils_scene::createCompositeCube(
        "floor",             // Name
        origin,              // Origin
        floorSize,           // Size
        oak_planks_material, // Material
        cubeVAO,             // VAO ID
        cubeIndexCount,      // Index count
        true                 // Is static
    );

    // wall X1
    glm::vec3 wallPosition1(0.0f, 1.0f, -1.0f);
    glm::vec3 wallSizeX(42.0f, 3.0f, 1.0f);
    utils_scene::createCompositeCube(
        "wall_X1",      // Name
        wallPosition1,  // Position
        wallSizeX,      // Size
        wallMaterial,   // Material
        cubeVAO,        // VAO ID
        cubeIndexCount, // Index count
        true            // Is static
    );

    // wall X2
    glm::vec3 wallPosition2(0.0f, 1.0f, 24.0f);
    utils_scene::createCompositeCube(
        "wall_X2",      // Name
        wallPosition2,  // Position
        wallSizeX,      // Size
        wallMaterial,   // Material
        cubeVAO,        // VAO ID
        cubeIndexCount, // Index count
        true            // Is static
    );

    // wall Z1
    glm::vec3 wallPosition3(-1.0f, 1.0f, 0.0f);
    glm::vec3 wallSizeZ1(1.0f, 3.0f, 24.0f);
    utils_scene::createCompositeCube(
        "wall_Z1",      // Name
        wallPosition3,  // Position
        wallSizeZ1,     // Size
        wallMaterial,   // Material
        cubeVAO,        // VAO ID
        cubeIndexCount, // Index count
        true            // Is static
    );

    // wall Z2
    glm::vec3 wallPosition4(42.0f, 1.0f, 0.0f);
    utils_scene::createCompositeCube(
        "wall_Z2",      // Name
        wallPosition4,  // Position
        wallSizeZ1,     // Size
        wallMaterial,   // Material
        cubeVAO,        // VAO ID
        cubeIndexCount, // Index count
        true            // Is static
    );

    // separation wall, Z3
    glm::vec3 wallPosition5(20.0f, 1.0f, 0.0f);
    glm::vec3 wallSizeZ2(2.0f, 3.0f, 10.0f);
    utils_scene::createCompositeCube(
        "wall_Z3",          // Name
        wallPosition5,      // Position
        wallSizeZ2,         // Size
        terracottaMaterial, // Material
        cubeVAO,            // VAO ID
        cubeIndexCount,     // Index count
        true                // Is static
    );

    // separation wall, Z4
    glm::vec3 wallPosition6(20.0f, 1.0f, 14.0f);
    utils_scene::createCompositeCube(
        "wall_Z4",          // Name
        wallPosition6,      // Position
        wallSizeZ2,         // Size
        terracottaMaterial, // Material
        cubeVAO,            // VAO ID
        cubeIndexCount,     // Index count
        true                // Is static
    );

    // transparent cube in room 2
    // utils_scene::addTransparentCube(
    //     "transparent_cube",           // Name
    //     transparentCubePosition,      // Position
    //     transparentCubeSize,          // Size
    //     transparent_iron_block_material,          // Material
    //     glm::vec3(0.0f, 1.0f, 0.0f),  // Rotation axis (Y-axis)
    //     0.0f,                         // Rotation angle
    //     cubeVAO,                      // VAO ID
    //     cubeIndexCount,               // Index count
    //     true                          // Is static
    // );

    // second transparent cube in room 2
    // glm::vec3 transparentCubePosition2(32.0f, 2.0f, 12.0f);
    // utils_scene::addTransparentCube(
    //     "transparent_cube2",            // Name
    //     transparentCubePosition2,       // Position
    //     transparentCubeSize,            // Size
    //     purple_stained_glass_material, // Material
    //     glm::vec3(0.0f, 1.0f, 0.0f),    // Rotation axis (Y-axis)
    //     0.0f,                           // Rotation angle
    //     cubeVAO,                        // VAO ID
    //     cubeIndexCount,                 // Index count
    //     true                            // Is static
    // );

    // third transparent cube in room 2
    // glm::vec3 transparentCubePosition(32.0f, 2.0f, 10.0f);
    // glm::vec3 transparentCubePosition3(33.0f, 1.501f, 11.5f);
    // glm::vec3 transparentCubeSize(10.0f, 2.0f, 10.0f);
    // utils_scene::addTransparentCube(
    //     "transparent_cube3",            // Name
    //     transparentCubePosition3,       // Position
    //     transparentCubeSize,            // Size
    //     glass_material, // Material
    //     glm::vec3(0.0f, 1.0f, 0.0f),    // Rotation axis (Y-axis)
    //     0.0f,                           // Rotation angle
    //     cubeVAO,                        // VAO ID
    //     cubeIndexCount,                 // Index count
    //     true                            // Is static
    // );

    // glm::vec3 transparentCubePosition4(27.0f, 1.001f, 7.0f);
    // glm::vec3 transparentCubeSize(10.0f, 2.0f, 10.0f);
    // utils_scene::createTransparentCompositeCube(
    //     "glassthing",          // Name
    //     transparentCubePosition4,      // Position
    //     transparentCubeSize,         // Size
    //     glass_material, // Material
    //     cubeVAO,            // VAO ID
    //     cubeIndexCount,     // Index count
    //     true                // Is static
    // );

    // 

    // load a soccer ball as sophisticated object
    // utils_scene::addSphere(
    //     "soccer_ball",                // Name
    //     glm::vec3(27.0f, 3.0f, 10.0f), // Position
    //     0.3f,                         // Radius
    //     soccerMaterial,               // Material
    //     sphereVAO,                    // VAO ID
    //     sphereVertexCount,            // Vertex count
    //     true                          // Is static
    // );

    // load skybox sphere
    utils_scene::addSkySphere(
        "sky",                       // Name
        glm::vec3(0.0f, 0.0f, 0.0f), // Position
        99.0f,                       // Radius
        skyMaterial,                 // Material
        sphereVAO,                   // VAO ID
        sphereVertexCount,           // Vertex count
        false                        // Is static
    );

    // planet balls
    // positions:
    // sun: 9.5f, 3.0f, 11.5f (size1 = 1.3f) size2 = 1.2f size3 = 1.1f
    // emrcury: 8.5f, 3.0f, 11.5f
    // venus: 7.5f, 3.0f, 11.5f
    // earth: 5.0f, 3.0f, 12.0f (size1 = 0.3f) size2 = 0.34f
    // mars: 3.5f, 3.0f, 11.5f
    // jupiter: 3.0f, 3.0f, 11.5f
    // saturn: -0.5f, 3.0f, 11.5f
    // uranus: -2.5f, 3.0f, 11.5f
    // neptune: -4.5f, 3.0f, 11.5f


    // sun1
    utils_scene::addTransparentSphere(
        "sun",                      // Name
        glm::vec3(9.5f, 34.0f, 11.5f), // Position
        3.3f,                       // Radius
        sunMaterial,                // Material
        sphereVAO,                  // VAO ID
        sphereVertexCount,          // Vertex count
        true                        // Is static
    );

    // mercury
    utils_scene::addSphere(
        "mercury",                  // Name
        glm::vec3(9.5f, 3.0f, 3.5f), // Position
        0.3f,                      // Radius
        mercuryMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // venus
    utils_scene::addSphere(
        "venus",                  // Name
        glm::vec3(9.5f, 3.0f, 4.5f), // Position
        0.34f,                      // Radius
        venusMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // venus atmosphere
    utils_scene::addTransparentSphere(
        "venus_atmosphere",                  // Name
        glm::vec3(10.5f, 3.0f, 4.5f), // Position
        0.36f,                      // Radius
        venusAtmosphereMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // earth
    utils_scene::addSphere(
        "earth",                  // Name
        glm::vec3(9.5f, 3.0f, 5.5f), // Position
        0.3f,                      // Radius
        earthMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // earth atmosphere
    utils_scene::addTransparentSphere(
        "earth_atmosphere",                  // Name
        glm::vec3(10.5f, 3.0f, 5.5f), // Position
        0.34f,                      // Radius
        earthAtmosphereMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // mars
    utils_scene::addSphere(
        "mars",                  // Name
        glm::vec3(9.5f, 3.0f, 6.5f), // Position
        0.3f,                      // Radius
        marsMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // jupiter
    utils_scene::addSphere(
        "jupiter",                  // Name
        glm::vec3(9.5f, 3.0f, 7.5f), // Position
        0.8f,                      // Radius
        jupiterMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // saturn
    utils_scene::addSphere(
        "saturn",                  // Name
        glm::vec3(9.5f, 3.0f, 9.5f), // Position
        0.7f,                      // Radius
        saturnMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // saturn ring
    utils_scene::addTransparentSphere(
        "saturn_ring",                  // Name
        glm::vec3(13.0f, 3.0f, 9.5f), // Position
        1.4f,                      // Radius
        saturnRingMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // uranus
    utils_scene::addSphere(
        "uranus",                  // Name
        glm::vec3(9.5f, 3.0f, 11.5f), // Position
        0.45f,                      // Radius
        uranusMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // neptune
    utils_scene::addSphere(
        "neptune",                  // Name
        glm::vec3(9.5f, 3.0f, 13.5f), // Position
        0.42f,                      // Radius
        neptuneMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // display cases we will add a glass cube with a planet inside, for each planet

    // mercury
    utils_scene::addTransparentCube(
        "mercury_case",                 // Name
        glm::vec3(4.0f, 1.501f, 3.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // venus
    utils_scene::addTransparentCube(
        "venus_case",                 // Name
        glm::vec3(8.0f, 1.501f, 3.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // earth
    utils_scene::addTransparentCube(
        "earth_case",                 // Name
        glm::vec3(12.0f, 1.501f, 3.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // mars
    utils_scene::addTransparentCube(
        "mars_case",                 // Name
        glm::vec3(16.0f, 1.501f, 3.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // right side:
    // jupiter
    utils_scene::addTransparentCube(
        "jupiter_case",                 // Name
        glm::vec3(16.0f, 1.501f, 20.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // saturn
    utils_scene::addTransparentCube(
        "saturn_case",                 // Name
        glm::vec3(12.0f, 1.501f, 20.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // uranus
    utils_scene::addTransparentCube(
        "uranus_case",                 // Name
        glm::vec3(8.0f, 1.501f, 20.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // neptune
    utils_scene::addTransparentCube(
        "neptune_case",                 // Name
        glm::vec3(4.0f, 1.501f, 20.0f),   // Position
        glm::vec3(2.0f, 2.0f, 2.0f),   // Size
        glass_material,                 // Material
        glm::vec3(0.0f, 1.0f, 0.0f),   // Rotation axis (Y-axis)
        0.0f,                          // Rotation angle
        cubeVAO,                       // VAO ID
        cubeIndexCount,                // Index count
        true                           // Is static
    );

    // =======================
    // now we can load the planets that go in each case

    // mercury
    utils_scene::addSphere(
        "mercury_display",         // Name
        glm::vec3(4.0f, 1.5f, 3.0f), // Position
        0.6f,                      // Radius
        mercuryMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // venus
    utils_scene::addSphere(
        "venus_display",         // Name
        glm::vec3(8.0f, 1.5f, 3.0f), // Position
        0.6f,                      // Radius
        venusMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // venus atmosphere
    utils_scene::addTransparentSphere(
        "venus_atmosphere_display",         // Name
        glm::vec3(8.0f, 1.5f, 3.0f), // Position
        0.62f,                      // Radius
        venusAtmosphereMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // earth
    utils_scene::addSphere(
        "earth_display",         // Name
        glm::vec3(12.0f, 1.5f, 3.0f), // Position
        0.6f,                      // Radius
        earthMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // earth atmosphere
    utils_scene::addTransparentSphere(
        "earth_atmosphere_display",         // Name
        glm::vec3(12.0f, 1.5f, 3.0f), // Position
        0.62f,                      // Radius
        earthAtmosphereMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // mars
    utils_scene::addSphere(
        "mars_display",         // Name
        glm::vec3(16.0f, 1.5f, 3.0f), // Position
        0.6f,                      // Radius
        marsMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // otehr side
    // jupiter
    utils_scene::addSphere(
        "jupiter_display",         // Name
        glm::vec3(16.0f, 1.5f, 20.0f), // Position
        0.6f,                      // Radius
        jupiterMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // saturn
    utils_scene::addSphere(
        "saturn_display",         // Name
        glm::vec3(12.0f, 1.5f, 20.0f), // Position
        0.6f,                      // Radius
        saturnMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // saturn ring
    utils_scene::addTransparentSphere(
        "saturn_ring_display",         // Name
        glm::vec3(12.0f, 1.5f, 20.0f), // Position
        1.1f,                      // Radius
        saturnRingMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // uranus
    utils_scene::addSphere(
        "uranus_display",         // Name
        glm::vec3(8.0f, 1.5f, 20.0f), // Position
        0.6f,                      // Radius
        uranusMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // neptune
    utils_scene::addSphere(
        "neptune_display",         // Name
        glm::vec3(4.0f, 1.5f, 20.0f), // Position
        0.6f,                      // Radius
        neptuneMaterial,           // Material
        sphereVAO,                 // VAO ID
        sphereVertexCount,         // Vertex count
        true                       // Is static
    );

    // // Load the Heater .obj model
    // utils_object::ModelData heaterModelData;
    // std::string modelPath = applicationPath.dirPath() + "assets/models/HeaterOBJ/Heater.obj";
    // if (!utils_object::loadOBJ(modelPath, applicationPath.dirPath() + "assets/models/HeaterOBJ/", heaterModelData))
    // {
    //     std::cerr << "Failed to load model heater" << std::endl;
    // }
    // else
    // {
    //     std::cout << "Heater Model Loaded: "
    //               << heaterModelData.vertices.size() / 3 << " vertices, "
    //               << heaterModelData.indices.size() << " indices." << std::endl;
    // }

    // // Set up OpenGL buffers for the model
    // setupModelBuffers(heaterModelData);

    // // Compute Bounding Box for the Model
    // AABB heaterModelBoundingBox = computeAABB(heaterModelData.vertices);

    // // Apply scale
    // glm::vec3 heaterModelScale(0.6f, 0.6f, 0.6f);
    // heaterModelBoundingBox.min *= heaterModelScale;
    // heaterModelBoundingBox.max *= heaterModelScale;

    // // Apply translation (position)
    // glm::vec3 heaterModelPosition(0.6f, 1.1f, 6.0f);
    // heaterModelBoundingBox.min += heaterModelPosition;
    // heaterModelBoundingBox.max += heaterModelPosition;

    // // Add the heater model with the material
    // utils_scene::addModel(
    //     "heater",                                             // Name
    //     heaterModelPosition,                                  // Position
    //     heaterModelScale,                                     // Scale
    //     heaterMaterial,                                       // Material
    //     heaterModelData.vao,                                  // VAO ID
    //     static_cast<GLsizei>(heaterModelData.indices.size()), // Index Count
    //     heaterModelBoundingBox,                               // Bounding Box
    //     glm::vec3(0.0f, 1.0f, 0.0f),                          // Rotation Axis (Y-axis)
    //     0.0f,                                                 // Rotation Angle
    //     true                                                  // Is static
    // );

    // Load the Rocking Chair model
    utils_object::ModelData rockingChairModelData;
    std::string rockingChairPath = applicationPath.dirPath() + "assets/models/Rocking_Chair/kid_rocking_chair.obj";
    std::string rockingChairBasePath = applicationPath.dirPath() + "assets/models/Rocking_Chair/Textures/";

    if (!utils_object::loadOBJ(rockingChairPath, rockingChairBasePath, rockingChairModelData))
    {
        std::cerr << "Failed to load Rocking Chair model." << std::endl;
    }
    else
    {
        std::cout << "Rocking Chair Model Loaded: "
                  << rockingChairModelData.vertices.size() / 3 << " vertices, "
                  << rockingChairModelData.indices.size() << " indices." << std::endl;
    }

    // Load Rocking Chair Base Color Texture
    std::string baseColorPath = rockingChairBasePath + "/BaseColor.png";
    GLuint baseColorTextureID = utils_object::LoadTextureFromFile(baseColorPath.c_str());
    if (baseColorTextureID == 0)
    {
        std::cerr << "Failed to load BaseColor.png for Rocking Chair." << std::endl;
    }
    else
    {
        std::cout << "BaseColor.png for Rocking Chair loaded successfully: ID " << baseColorTextureID << std::endl;
    }

    // Load Rocking Chair Normal Map
    std::string normalMapPath = rockingChairBasePath + "/Normal.png";
    GLuint normalMapTextureID = utils_object::LoadTextureFromFile(normalMapPath.c_str());
    if (normalMapTextureID == 0)
    {
        std::cerr << "Failed to load Normal.png for Rocking Chair." << std::endl;
    }
    else
    {
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
    glm::vec3 rockingChairModelPosition(10.0f, 0.55f, 22.5f);
    rockingChairModelBoundingBox.min += rockingChairModelPosition;
    rockingChairModelBoundingBox.max += rockingChairModelPosition;

    // Add Rocking Chair Model to Scene Objects
    utils_scene::addModel(
        "rocking_chair",                                            // Name
        rockingChairModelPosition,                                  // Position
        rockingChairModelScale,                                     // Scale
        rockingChairMaterial,                                       // Material
        rockingChairModelData.vao,                                  // VAO ID
        static_cast<GLsizei>(rockingChairModelData.indices.size()), // Index Count
        rockingChairModelBoundingBox,                               // Bounding Box
        glm::vec3(0.0f, 0.0f, 0.0f),                                // Rotation Axis (no rotation)
        0.0f,                                                       // Rotation Angle
        false                                                       // Is static
    );

    // add the torus model
    utils_object::ModelData torusModelData;

    std::string torusPath = applicationPath.dirPath() + "assets/models/Torus/Torus.obj";
    std::string torusBasePath = applicationPath.dirPath() + "assets/models/Torus/";

    if (!utils_object::loadOBJ(torusPath, torusBasePath, torusModelData))
    {
        std::cerr << "Failed to load Torus model." << std::endl;
    }
    else
    {
        std::cout << "Torus Model Loaded: "
                  << torusModelData.vertices.size() / 3 << " vertices, "
                  << torusModelData.indices.size() << " indices." << std::endl;
    }

    setupModelBuffers(torusModelData);

    AABB torusBoundingBox = computeAABB(torusModelData.vertices);
    // replace bounding box with new scale

    glm::vec3 torusPosition(29.0f, 1.0f, 21.0f);
    glm::vec3 torusScale(0.05f, 0.05f, 0.05f);

    torusBoundingBox.min *= torusScale;
    torusBoundingBox.max *= torusScale;

    torusBoundingBox.min += torusPosition;
    torusBoundingBox.max += torusPosition;

    utils_scene::addModel(
        "torus",                                             // Name
        torusPosition,                                       // Position
        torusScale,                                          // Scale
        torusMaterial,                                       // Material
        torusModelData.vao,                                  // VAO ID
        static_cast<GLsizei>(torusModelData.indices.size()), // Index Count
        torusBoundingBox,                                    // Bounding Box
        glm::vec3(0.0f, 1.0f, 0.0f),                         // Rotation Axis (Y-axis)
        0.0f,                                                // Rotation Angle
        true                                                 // Is static
    );

    // add a std::vector of simple point lights from namespace utils_light
    std::vector<utils_light::SimplePointLight> simpleLights;

    // Add a simple point light to the scene
    // int newLightID = utils_light::addLight(
    //     simpleLights,
    //     glm::vec3(5.0f, 2.5f, 15.0f), // position
    //     glm::vec3(1.0f, 1.0f, 1.0f),  // color
    //     1.0f                          // intensity
    // );

    // // // Add a simple point light to the scene 2
    // int newLightID2 = utils_light::addLight(
    //     simpleLights,
    //     glm::vec3(5.0f, 1.0f, 19.0f), // position
    //     glm::vec3(0.3f, 0.4f, 1.0f),  // color
    //     1.0f                          // intensity
    // );

    // // light pos 32 2 11
    // int newLightID3 = utils_light::addLight(
    //     simpleLights,
    //     glm::vec3(32.0f, 2.0f, 19.0f), // position
    //     glm::vec3(1.0f, 0.0f, 1.0f),   // color
    //     1.0f                           // intensity
    // );

    // // light pos 32 2 13
    // int newLightID4 = utils_light::addLight(
    //     simpleLights,
    //     glm::vec3(37.0f, 2.0f, 13.0f), // position
    //     glm::vec3(1.0f, 1.0f, 1.0f),   // color
    //     1.0f                           // intensity
    // );

    // // pos 10 1 10
    // int newLightID5 = utils_light::addLight(
    //     simpleLights,
    //     glm::vec3(10.0f, 1.0f, 10.0f), // position
    //     glm::vec3(1.0f, 0.0f, 1.0f),   // color
    //     1.0f                           // intensity
    // );

    // one light for each planet case, a little bit above the planet
    // mercury
    int newLightID1 = utils_light::addLight(
        simpleLights,
        glm::vec3(4.0f, 3.0f, 3.0f), // position
        glm::vec3(1.0f, 1.0f, 1.0f),  // color
        0.2f                          // intensity
    );

    // venus
    int newLightID2 = utils_light::addLight(
        simpleLights,
        glm::vec3(8.0f, 3.0f, 3.0f), // position
        glm::vec3(1.0f, 1.0f, 1.0f),  // color
        0.2f                        // intensity
    );

    // earth
    int newLightID3 = utils_light::addLight(
        simpleLights,
        glm::vec3(12.0f, 3.0f, 3.0f), // position
        glm::vec3(1.0f, 1.0f, 1.0f),  // color
        0.2f                          // intensity
    );

    // mars
    int newLightID4 = utils_light::addLight(
        simpleLights,
        glm::vec3(16.0f, 3.0f, 3.0f), // position
        glm::vec3(1.0f, 1.0f, 1.0f),  // color
        0.2f                          // intensity
    );

    // jupiter
    int newLightID5 = utils_light::addLight(
        simpleLights,
        glm::vec3(16.0f, 3.0f, 20.0f), // position
        glm::vec3(1.0f, 1.0f, 1.0f),   // color
        0.2f                           // intensity
    );

    // saturn
    int newLightID6 = utils_light::addLight(
        simpleLights,
        glm::vec3(12.0f, 3.0f, 20.0f), // position
        glm::vec3(1.0f, 1.0f, 1.0f),   // color
        0.2f                           // intensity
    );

    // uranus
    int newLightID7 = utils_light::addLight(
        simpleLights,
        glm::vec3(8.0f, 3.0f, 20.0f), // position
        glm::vec3(1.0f, 1.0f, 1.0f),   // color
        0.2f                           // intensity
    );

    // neptune
    int newLightID8 = utils_light::addLight(
        simpleLights,
        glm::vec3(4.0f, 3.0f, 20.0f), // position
        glm::vec3(1.0f, 1.0f, 1.0f),   // color
        0.2f                           // intensity
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

    utils_scene::initializePlanetSpiralParameters();

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
        std::string newTitle = "Boules - FPS: " + std::to_string(fps) + " - Position: (" + std::to_string(cameraPos.x) + ", " + std::to_string(cameraPos.z) + ")";
        // std::string newTitle = std::to_string(cameraPos.x) + ", " + std::to_string(cameraPos.z);
        // std::string newTitle = "FPS: " + std::to_string(fps);
        SDL_SetWindowTitle(windowManager.getWindow(), newTitle.c_str());

        // Update light intensity dynamically within the loop
        // glm::vec3 lightIntensity = glm::vec3(
        //     (sin(currentFrame) + 1.0f) / 2.0f,       // Red oscillates between 0 and 1
        //     (cos(currentFrame) + 1.0f) / 2.0f,       // Green oscillates between 0 and 1
        //     (sin(currentFrame * 0.5f) + 1.0f) / 2.0f // Blue oscillates more slowly between 0 and 1
        // );

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

        // Calculate proposed camera direction based on input
        glm::vec3 movementDirection = glm::vec3(0.0f);

        // Forward and Backward
        if (state[SDL_SCANCODE_W])
        {
            movementDirection += frontDirection;
        }
        if (state[SDL_SCANCODE_S])
        {
            movementDirection -= frontDirection;
        }

        // Left and Right
        if (state[SDL_SCANCODE_A])
        {
            movementDirection -= rightDirection;
        }
        if (state[SDL_SCANCODE_D])
        {
            movementDirection += rightDirection;
        }

        // Normalize to prevent diagonal speed boost
        if (glm::length(movementDirection) > 0.0f)
        {
            movementDirection = glm::normalize(movementDirection);
        }

        // Apply movement speed
        proposedCameraPos += movementDirection * adjustedSpeed;

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
        // Check collision against all objects with transparency
        for (const auto &object : utils_scene::sceneObjectsTransparent)
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



        // Spiral movement around spiralCenter

        // Light properties
        // float spiralRadius = 2.0f; // Radius of the spiral
        // float spiralSpeed = 0.5f;  // Speed of the spiral movement
        // float fixedHeight = 1.5f;  // Height variation of the light
        // glm::vec3 lightPosWorld;
        // lightPosWorld.x = spiralCenter.x + spiralRadius * cos(currentFrame * spiralSpeed);
        // lightPosWorld.y = fixedHeight + spiralCenter.y;
        // lightPosWorld.z = spiralCenter.z + spiralRadius * sin(currentFrame * spiralSpeed);

        // fixed light position
        glm::vec3 lightPosWorld;

        // new method for light color, set to its material Kd
        glm::vec3 lightIntensity = lightMaterial.Kd * 1.0f;

        // simple light ID 5, dynamic light
        // glm::vec3 lightPosWorld5;
        // lightPosWorld5.x = 10.0f + 1.0f * cos(currentFrame);
        // lightPosWorld5.y = 1.0f;
        // lightPosWorld5.z = 10.0f + 1.0f * sin(currentFrame);

        // simpleLights[4].position = lightPosWorld5;

        // also change its color
        // simpleLights[4].color = glm::vec3(
        //     (sin(currentFrame) + 1.0f) / 2.0f,       // Red oscillates between 0 and 1
        //     (cos(currentFrame) + 1.0f) / 2.0f,       // Green oscillates between 0 and 1
        //     (sin(currentFrame * 0.5f) + 1.0f) / 2.0f // Blue oscillates more slowly between 0 and 1
        // );

        // is light paused
        // default sun position glm::vec3(9.5f, 34.0f, 11.5f)
        // if light is paused, move the sunposition down 100 units
        // else, bakc to the default original position
        if (isLightPaused)
        {
            // setobject position to 100 units below the scene
            utils_scene::setObjectPosition("sun", glm::vec3(9.5f, -100.0f, 11.5f));
        }
        else
        {
            // setobject position to the original position
            utils_scene::setObjectPosition("sun", glm::vec3(9.5f, 34.0f, 11.5f));
        }

        // set the main light position to the sun position
        // get the position of the sun using the transparent lsit of objects, name is "sun"
        // Fetch sun's position dynamically
        glm::vec3 sunPosition = utils_scene::getTransparentObjectPosition("sun");
        lightPosWorld = sunPosition;

        glm::vec3 spiralCenter = utils_scene::getTransparentObjectPosition("sun");

        utils_scene::updatePlanetPositions(currentFrame, spiralCenter);


        // update the display lights
        utils_light::updateDynamicLights(simpleLights, currentFrame);

        // update the display planets (they should rotate)
        utils_scene::updateDisplayPlanetPositions(currentFrame);


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
        double frequency = 0.30; // Rocking frequency (cycles per second)
        double length = 0.08;    // Maximum angle in degrees
        double radius = 0.3;     // Radius of the rocking base

        // dynamic loop
        utils_game_loop::dynamic_loop(deltaTime, lastFrame, currentFrame, windowManager, cameraPos, cameraFront, cameraUp, cameraSpeed, done, isRockingChairPaused, rockingChairStartTime, rockingChairPausedTime, yaw, pitch, radius, frequency, radius, length, cameraHeight);

        if (!isLightPaused)
        {

            // First Pass: Render scene to depth cube map
            for (unsigned int i = 0; i < 6; ++i)
            {
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
                for (const auto &object : utils_scene::sceneObjects)
                {
                    glm::mat4 modelMatrix = glm::mat4(1.0f);
                    modelMatrix = glm::translate(modelMatrix, object.position);
                    if (object.rotationAngle != 0.0f)
                    {
                        modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotationAngle), object.rotationAxis);
                    }
                    modelMatrix = glm::scale(modelMatrix, object.scale);

                    // Set model matrix for depth shader
                    glUniformMatrix4fv(glGetUniformLocation(depthShader.getGLId(), "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

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
                    glBindVertexArray(0);
                }

                // Optional: Unbind the framebuffer after each face
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }

        } // end of if (!isLightPaused)
        else
        {
            // set intensity to 0
            lightIntensity = glm::vec3(0.0f);
            // next loop will set back to Kd of the material, unless light is still paused
        }

        // Second Pass: Render the scene normally with point light
        glViewport(0, 0, window_width, window_height);
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // back to default
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set the shader program to use, room1 for x udner 20.5 and room2 for x over 20.5
        utils_loader::Shader *currentRoom = &room1;
        bool inRoom2 = cameraPos.x >= 20.5f;
        if (inRoom2)
        {
            currentRoom = &room2;
        }

        currentRoom->use();

        // Retrieve uniform locations specific to the active shader
        GLint uModelMatrixLocation = currentRoom->getUniformLocation("uModelMatrix");
        GLint uMVPMatrixLocation = currentRoom->getUniformLocation("uMVPMatrix");
        GLint uMVMatrixLocation = currentRoom->getUniformLocation("uMVMatrix");
        GLint uNormalMatrixLocation = currentRoom->getUniformLocation("uNormalMatrix");
        GLint uTextureLocation = currentRoom->getUniformLocation("uTexture");
        GLint uUseTextureLocation = currentRoom->getUniformLocation("uUseTexture");
        GLint uKdLocation = currentRoom->getUniformLocation("uKd");
        GLint uKsLocation = currentRoom->getUniformLocation("uKs");
        GLint uShininessLocation = currentRoom->getUniformLocation("uShininess");
        GLint uLightPos_vsLocation = currentRoom->getUniformLocation("uLightPos_vs");
        GLint uLightIntensityLocation = currentRoom->getUniformLocation("uLightIntensity");

        // Retrieve 'uAlpha' only if in room2 (deprecated, we add transparency to room 1 as well)
        // GLint uAlphaLocation = -1;
        // if (inRoom2) {
        //     uAlphaLocation = currentRoom->getUniformLocation("uAlpha");
        //     if (uAlphaLocation == -1) {
        //         std::cerr << "Failed to get 'uAlpha' location in room2 shader" << std::endl;
        //     }
        // }

        // Retrieve 'uAlpha' regardless of room
        GLint uAlphaLocation = currentRoom->getUniformLocation("uAlpha");

        // Retrieve additional uniforms for Shader 2
        GLint uNormalMapLocation = currentRoom->getUniformLocation("uNormalMap");
        GLint uUseNormalMapLocation = currentRoom->getUniformLocation("uUseNormalMap");
        GLint uSpecularMapLocation = currentRoom->getUniformLocation("uSpecularMap");
        GLint uUseSpecularMapLocation = currentRoom->getUniformLocation("uUseSpecularMap");

        // Determine the number of additional lights, capped by MAX_ADDITIONAL_LIGHTS
        int numLights = static_cast<int>(simpleLights.size());
        if (numLights > MAX_ADDITIONAL_LIGHTS)
        {
            numLights = MAX_ADDITIONAL_LIGHTS;
        }

        // Set the number of additional lights in the shader
        GLint numLightsLoc = glGetUniformLocation(currentRoom->getGLId(), "uNumAdditionalLights");
        glUniform1i(numLightsLoc, numLights);

        // Convert additional light positions to view space
        std::vector<glm::vec3> additionalLightPosViewSpace;
        additionalLightPosViewSpace.reserve(numLights);
        for (int i = 0; i < numLights; ++i)
        {
            glm::vec4 posView = ViewMatrix * glm::vec4(simpleLights[i].position, 1.0f);
            additionalLightPosViewSpace.emplace_back(glm::vec3(posView));
        }

        // Set additional light positions and colors once per frame
        for (int i = 0; i < numLights; ++i)
        {
            std::string idx = std::to_string(i);

            // Position
            GLint posLoc = glGetUniformLocation(
                currentRoom->getGLId(),
                ("uAdditionalLightPos[" + idx + "]").c_str());
            glUniform3fv(posLoc, 1, glm::value_ptr(additionalLightPosViewSpace[i]));

            // Color
            GLint colorLoc = glGetUniformLocation(
                currentRoom->getGLId(),
                ("uAdditionalLightColor[" + idx + "]").c_str());
            glUniform3fv(colorLoc, 1, glm::value_ptr(simpleLights[i].color));

            // Initialize intensity to zero; will be set per object
            GLint intenLoc = glGetUniformLocation(
                currentRoom->getGLId(),
                ("uAdditionalLightIntensity[" + idx + "]").c_str());
            glUniform1f(intenLoc, 0.0f);
        }

        // Set main light properties once per frame
        glUniform3fv(uLightPos_vsLocation, 1, glm::value_ptr(lightPosViewSpace));
        glUniform3fv(uLightIntensityLocation, 1, glm::value_ptr(lightIntensity));

        // Set the updated light space matrix
        glUniformMatrix4fv(glGetUniformLocation(currentRoom->getGLId(), "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // Bind the depth cube map to texture unit 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
        glUniform1i(glGetUniformLocation(currentRoom->getGLId(), "depthMap"), 1);

        // Set light and camera positions in world space
        glUniform3fv(glGetUniformLocation(currentRoom->getGLId(), "lightPosWorld"), 1, glm::value_ptr(lightPosWorld));
        glUniform3fv(glGetUniformLocation(currentRoom->getGLId(), "cameraPosWorld"), 1, glm::value_ptr(cameraPos));

        // Set far plane
        glUniform1f(glGetUniformLocation(currentRoom->getGLId(), "farPlane"), farPlane);

        // **Sort Transparent Objects Back-to-Front**
        if (inRoom2 && !utils_scene::sceneObjectsTransparent.empty())
        {
            std::sort(utils_scene::sceneObjectsTransparent.begin(), utils_scene::sceneObjectsTransparent.end(),
                    [&](const utils_scene::SceneObject &a, const utils_scene::SceneObject &b)
                    {
                        float distanceA = glm::length(cameraPos - a.position);
                        float distanceB = glm::length(cameraPos - b.position);
                        return distanceA > distanceB;
                    });
        }

        glDisable(GL_CULL_FACE);

        auto deltaObj = 0.0f;
        auto deltaLight = 0.0f;
        bool sameRoom = false;

        GLint uTimeLocation = currentRoom->getUniformLocation("uTime");
        if (uTimeLocation != -1) {
            glUniform1f(uTimeLocation, currentFrame);
        }

        // Render all scene objects (opaque)
        for (const auto &object : utils_scene::sceneObjects)
        {
            // Setup model matrix
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, object.position);
            if (object.rotationAngle != 0.0f)
            {
                modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotationAngle), object.rotationAxis);
            }
            modelMatrix = glm::scale(modelMatrix, object.scale);

            // Calculate matrices
            glm::mat4 mvMatrix = ViewMatrix * modelMatrix;
            glm::mat4 mvpMatrix = ProjMatrix * mvMatrix;
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix)));

            // Set uniforms for shaders
            glUniformMatrix4fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
            glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix3fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            // Determine if the object is in the same room as the main light
            float deltaObj   = 20.5f - object.position.x;
            float deltaLight = 20.5f - lightPosWorld.x;
            bool sameRoom    = (deltaObj * deltaLight) > 0; // true if object & light are on the same side

            if (sameRoom) {
                // Main light affects this object
                glUniform3fv(uLightIntensityLocation, 1, glm::value_ptr(lightIntensity));
            } else {
                // Main light does not affect this object
                glUniform3fv(uLightIntensityLocation, 1, glm::value_ptr(glm::vec3(0.0f)));
            }

            // **Additional Lights Uniforms**
            for (int i = 0; i < numLights; ++i)
            {
                float deltaObjForAddLight   = 20.5f - object.position.x;
                float deltaAdditionalLight  = 20.5f - simpleLights[i].position.x;
                bool sameRoomForAddLight    = (deltaObjForAddLight * deltaAdditionalLight) > 0;

                std::string idx = std::to_string(i);
                GLint intenLoc  = glGetUniformLocation(currentRoom->getGLId(), ("uAdditionalLightIntensity[" + idx + "]").c_str());
                // also position
                GLint posLoc    = glGetUniformLocation(currentRoom->getGLId(), ("uAdditionalLightPos[" + idx + "]").c_str());

                if (sameRoomForAddLight)
                {
                    // Enable this additional light for the object
                    glUniform1f(intenLoc, simpleLights[i].intensity);
                }
                else
                {
                    // Disable this additional light for the object
                    glUniform1f(intenLoc, 0.0f);
                }
            }

            // Retrieve the material from the manager
            const Material &mat = materialManager.getMaterial(object.materialIndex);

            // 1) Diffuse color
            if (uKdLocation != -1)
            {
                glUniform3fv(uKdLocation, 1, glm::value_ptr(mat.Kd));
            }

            // 2) Specular color
            if (uKsLocation != -1)
            {
                glUniform3fv(uKsLocation, 1, glm::value_ptr(mat.Ks));
            }

            // 3) Shininess
            if (uShininessLocation != -1)
            {
                glUniform1f(uShininessLocation, mat.shininess);
            }

            // 4) Alpha
            if (uAlphaLocation != -1)
            {
                glUniform1f(uAlphaLocation, mat.alpha);
            }

            // Bind textures if applicable
            if (mat.hasDiffuseMap && mat.diffuseMapID != 0 && uUseTextureLocation != -1)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mat.diffuseMapID);
                glUniform1i(uTextureLocation, 0);
                glUniform1f(uUseTextureLocation, 1.0f);
            }
            else
            {
                if (uUseTextureLocation != -1)
                {
                    glUniform1f(uUseTextureLocation, 0.0f);
                }
            }

            // Bind normal map if applicable
            if (mat.hasNormalMap && mat.normalMapID != 0 && uUseNormalMapLocation != -1)
            {
                glActiveTexture(GL_TEXTURE2); // Use texture unit 2 for normal maps
                glBindTexture(GL_TEXTURE_2D, mat.normalMapID);
                glUniform1i(uNormalMapLocation, 2);
                glUniform1f(uUseNormalMapLocation, 1.0f);
            }
            else
            {
                if (uUseNormalMapLocation != -1)
                {
                    glUniform1f(uUseNormalMapLocation, 0.0f);
                }
            }

            // Bind specular map if applicable
            if (mat.hasSpecularMap && mat.specularMapID != 0 && uUseSpecularMapLocation != -1)
            {
                glActiveTexture(GL_TEXTURE3); // Use texture unit 3 for specular maps
                glBindTexture(GL_TEXTURE_2D, mat.specularMapID);
                glUniform1i(uSpecularMapLocation, 3);
                glUniform1f(uUseSpecularMapLocation, 1.0f);
            }
            else
            {
                if (uUseSpecularMapLocation != -1)
                {
                    glUniform1f(uUseSpecularMapLocation, 0.0f);
                }
            }

            // Bind VAO and draw the object
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
            glBindVertexArray(0);
        }


        // =======================
        // Render Skybox Objects
        skyboxShader.use();

        for (const auto &object : utils_scene::sceneObjectsSkybox)
        {
            // print object name
            // std::cout << "Object Name: " << object.name << std::endl;
            // Transformation Matrices
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

            // Set Skybox Shader Uniforms
            glUniformMatrix4fv(sky_uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix4fv(sky_uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
            glUniformMatrix3fv(sky_uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            // Retrieve the material for the object
            const Material &mat = materialManager.getMaterial(object.materialIndex);

            // 1) Diffuse Color
            if (sky_uKdLocation != -1)
            {
                glUniform3fv(sky_uKdLocation, 1, glm::value_ptr(mat.Kd));
            }

            // 2) Specular Color
            if (sky_uKsLocation != -1)
            {
                glUniform3fv(sky_uKsLocation, 1, glm::value_ptr(mat.Ks));
            }

            // 3) Shininess
            if (sky_uShininessLocation != -1)
            {
                glUniform1f(sky_uShininessLocation, mat.shininess);
            }

            // 4) Alpha (Transparency)
            GLint sky_uAlphaLocation = glGetUniformLocation(skyboxShader.getGLId(), "uAlpha");
            if (sky_uAlphaLocation != -1)
            {
                glUniform1f(sky_uAlphaLocation, mat.alpha);
            }

            // 5) Diffuse Map
            if (mat.hasDiffuseMap && mat.diffuseMapID != 0)
            {
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, mat.diffuseMapID);
                if (sky_uTextureLocation != -1)
                {
                    glUniform1i(sky_uTextureLocation, 0);
                }
                if (sky_uUseTextureLocation != -1)
                {
                    glUniform1f(sky_uUseTextureLocation, 1.0f);
                }
            }
            else
            {
                if (sky_uUseTextureLocation != -1)
                {
                    glUniform1f(sky_uUseTextureLocation, 0.0f);
                }
            }

            // 6) Normal Map
            GLint sky_uNormalMapLocation = glGetUniformLocation(skyboxShader.getGLId(), "uNormalMap");
            GLint sky_uUseNormalMapLocation = glGetUniformLocation(skyboxShader.getGLId(), "uUseNormalMap");
            if (mat.hasNormalMap && mat.normalMapID != 0)
            {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, mat.normalMapID);
                if (sky_uNormalMapLocation != -1)
                {
                    glUniform1i(sky_uNormalMapLocation, 2);
                }
                if (sky_uUseNormalMapLocation != -1)
                {
                    glUniform1f(sky_uUseNormalMapLocation, 1.0f);
                }
            }
            else
            {
                if (sky_uUseNormalMapLocation != -1)
                {
                    glUniform1f(sky_uUseNormalMapLocation, 0.0f);
                }
            }

            // 7) Specular Map
            GLint sky_uSpecularMapLocation = glGetUniformLocation(skyboxShader.getGLId(), "uSpecularMap");
            GLint sky_uUseSpecularMapLocation = glGetUniformLocation(skyboxShader.getGLId(), "uUseSpecularMap");
            if (mat.hasSpecularMap && mat.specularMapID != 0)
            {
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, mat.specularMapID);
                if (sky_uSpecularMapLocation != -1)
                {
                    glUniform1i(sky_uSpecularMapLocation, 3);
                }
                if (sky_uUseSpecularMapLocation != -1)
                {
                    glUniform1f(sky_uUseSpecularMapLocation, 1.0f);
                }
            }
            else
            {
                if (sky_uUseSpecularMapLocation != -1)
                {
                    glUniform1f(sky_uUseSpecularMapLocation, 0.0f);
                }
            }

            // Draw Skybox Object
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

            // Unbind Textures
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glBindVertexArray(0);
        glUseProgram(0);

        // =======================

        // back to previosu shader

        lightShader.use();
    
        if (lightIntensity != glm::vec3(0.0f))
        {
            // material is lightMaterial
            const Material &mat = lightMaterial;

            // Create model matrix with translation and scaling
            glm::mat4 modelMatrix = glm::mat4(1.0f);

            // Translate the sphere to the light source position
            modelMatrix = glm::translate(modelMatrix, lightPosWorld);

            // Scale the sphere to make it smaller
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f)); // Scale down to 10%

            // Calculate matrices
            glm::mat4 mvMatrix = ViewMatrix * modelMatrix;
            glm::mat4 mvpMatrix = ProjMatrix * mvMatrix;
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix)));

            // Pass matrices to the shader
            glUniformMatrix4fv(light_uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix4fv(light_uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
            glUniformMatrix3fv(light_uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            // Retrieve the material for the object

            // 1) Diffuse Color
            if (light_uKdLocation != -1)
            {
                glUniform3fv(light_uKdLocation, 1, glm::value_ptr(mat.Kd));
            }

            // Draw the smaller sphere
            glBindVertexArray(sphereVAO);
            glDrawArrays(GL_TRIANGLES, 0, sphereVertexCountGL);
            glBindVertexArray(0);
        }

        glUseProgram(0);

        lightShader.use();

        // Set material properties once for all point lights
        glUniform3fv(light_uKdLocation, 1, glm::value_ptr(simpleLightMaterial.Kd));
        glUniform3fv(light_uKsLocation, 1, glm::value_ptr(simpleLightMaterial.Ks));
        glUniform1f(light_uShininessLocation, simpleLightMaterial.shininess);
        glUniform1f(glGetUniformLocation(lightShader.getGLId(), "uAlpha"), simpleLightMaterial.alpha);

        for (const auto &light : simpleLights)
        {
            // Always render the sphere if you like, or skip if intensity is 0
            if (light.intensity <= 0.0f)
                continue;

            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, light.position);
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));

            glm::mat4 mvMatrix     = ViewMatrix * modelMatrix;
            glm::mat4 mvpMatrix    = ProjMatrix * mvMatrix;
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix)));

            // Pass transformation matrices
            glUniformMatrix4fv(light_uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix4fv(light_uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
            glUniformMatrix3fv(light_uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            // Optional: per-light color
            glUniform3fv(light_uKdLocation, 1, glm::value_ptr(light.color));

            // Render light sphere
            glBindVertexArray(sphereVAO);
            glDrawArrays(GL_TRIANGLES, 0, sphereVertexCountGL);
            glBindVertexArray(0);
        }

        glUseProgram(0);

        currentRoom->use();

        // Disable face culling to render both front and back faces
        // glDisable(GL_CULL_FACE);

        // Render all transparent objects
        if (!utils_scene::sceneObjectsTransparent.empty())
        {
            // Enable blending
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            // Disable depth writing so transparent objects blend with background
            glDepthMask(GL_FALSE);

            for (const auto &object : utils_scene::sceneObjectsTransparent)
            {
                // Check material index validity
                if (object.materialIndex < 0 || object.materialIndex >= static_cast<int>(materialManager.materials.size()))
                {
                    std::cerr << "Invalid material index for object: " << object.name << std::endl;
                    continue; // Skip rendering this object
                }

                // **Main Light** (object vs. main light) for transparency
                float deltaObjectTrans = 20.5f - object.position.x;
                float deltaMainLight   = 20.5f - lightPosWorld.x;
                bool sameRoomTrans     = (deltaObjectTrans * deltaMainLight) > 0;

                if (sameRoomTrans)
                {
                    glUniform3fv(uLightPos_vsLocation, 1, glm::value_ptr(lightPosViewSpace));
                    glUniform3fv(uLightIntensityLocation, 1, glm::value_ptr(lightIntensity));
                }
                else
                {
                    glUniform3fv(uLightIntensityLocation, 1, glm::value_ptr(glm::vec3(0.0f)));
                }

                // **Additional Lights** (object vs. each additional light)
                int numLights = (int)simpleLights.size();
                for (int i = 0; i < numLights; ++i)
                {
                    float deltaObjForAddLight   = 20.5f - object.position.x;
                    float deltaAdditionalLight  = 20.5f - simpleLights[i].position.x;
                    bool sameRoomForAddLight    = (deltaObjForAddLight * deltaAdditionalLight) > 0;

                    std::string idx  = std::to_string(i);
                    GLint intenLoc   = glGetUniformLocation(currentRoom->getGLId(), ("uAdditionalLightIntensity[" + idx + "]").c_str());

                    if (sameRoomForAddLight)
                    {
                        // Position
                        GLint posLoc = glGetUniformLocation(currentRoom->getGLId(),
                                        ("uAdditionalLightPos[" + idx + "]").c_str());
                        glUniform3fv(posLoc, 1, glm::value_ptr(additionalLightPosViewSpace[i]));

                        // Color
                        GLint colorLoc = glGetUniformLocation(currentRoom->getGLId(),
                                        ("uAdditionalLightColor[" + idx + "]").c_str());
                        glUniform3fv(colorLoc, 1, glm::value_ptr(simpleLights[i].color));

                        // Intensity
                        glUniform1f(intenLoc, simpleLights[i].intensity);
                    }
                    else
                    {
                        glUniform1f(intenLoc, 0.0f);
                    }
                }

                // Retrieve the material
                const Material &mat = materialManager.getMaterial(object.materialIndex);

                // **Alpha Check for Special Opaque-like Rendering (alpha == 0.9)**
                if (abs(mat.alpha - 0.9f) < 0.001f) {
                    // Treat object as opaque
                    glEnable(GL_CULL_FACE); // Enable face culling to prevent inside rendering
                    glDepthMask(GL_TRUE);   // Enable depth writing for proper occlusion
                    glDisable(GL_BLEND);    // Disable blending for solid rendering
                } else {
                    // Standard transparency handling
                    glDisable(GL_CULL_FACE); // Disable face culling for transparency
                    glDepthMask(GL_FALSE);   // Disable depth writing for blending
                    glEnable(GL_BLEND);      // Ensure blending is enabled
                }

                // Set alpha uniform
                if (uAlphaLocation != -1) {
                    glUniform1f(uAlphaLocation, mat.alpha);
                }

                // Calculate model matrix
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

                // Set transform uniforms
                glUniformMatrix4fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
                glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
                glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
                glUniformMatrix3fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

                // 1) Diffuse color
                if (uKdLocation != -1)
                {
                    glUniform3fv(uKdLocation, 1, glm::value_ptr(mat.Kd));
                }

                // 2) Specular color
                if (uKsLocation != -1)
                {
                    glUniform3fv(uKsLocation, 1, glm::value_ptr(mat.Ks));
                }

                // 3) Shininess
                if (uShininessLocation != -1)
                {
                    glUniform1f(uShininessLocation, mat.shininess);
                }

                // 4) Diffuse texture
                if (mat.hasDiffuseMap && mat.diffuseMapID != 0)
                {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, mat.diffuseMapID);
                    if (uTextureLocation != -1)
                    {
                        glUniform1i(uTextureLocation, 0);
                    }
                    if (uUseTextureLocation != -1)
                    {
                        glUniform1f(uUseTextureLocation, 1.0f);
                    }
                }
                else
                {
                    if (uUseTextureLocation != -1)
                    {
                        glUniform1f(uUseTextureLocation, 0.0f);
                    }
                }

                // 5) Normal map
                GLint uNormalMapLoc = glGetUniformLocation(currentRoom->getGLId(), "uNormalMap");
                GLint uUseNormalMapLoc = glGetUniformLocation(currentRoom->getGLId(), "uUseNormalMap");
                if (mat.hasNormalMap && mat.normalMapID != 0)
                {
                    glActiveTexture(GL_TEXTURE2);
                    glBindTexture(GL_TEXTURE_2D, mat.normalMapID);
                    if (uNormalMapLoc != -1)
                    {
                        glUniform1i(uNormalMapLoc, 2);
                    }
                    if (uUseNormalMapLoc != -1)
                    {
                        glUniform1f(uUseNormalMapLoc, 1.0f);
                    }
                }
                else
                {
                    if (uUseNormalMapLoc != -1)
                    {
                        glUniform1f(uUseNormalMapLoc, 0.0f);
                    }
                }

                // 6) Specular map
                GLint uSpecularMapLocation = glGetUniformLocation(currentRoom->getGLId(), "uSpecularMap");
                GLint uUseSpecularMapLocation = glGetUniformLocation(currentRoom->getGLId(), "uUseSpecularMap");
                if (mat.hasSpecularMap && mat.specularMapID != 0)
                {
                    glActiveTexture(GL_TEXTURE3); // Use texture unit 1 for specular maps
                    glBindTexture(GL_TEXTURE_2D, mat.specularMapID);
                    if (uSpecularMapLocation != -1)
                    {
                        glUniform1i(uSpecularMapLocation, 3); // Set sampler to texture unit 3
                    }
                    if (uUseSpecularMapLocation != -1)
                    {
                        glUniform1f(uUseSpecularMapLocation, 1.0f); // Enable specular map usage
                    }
                }
                else
                {
                    if (uUseSpecularMapLocation != -1)
                    {
                        glUniform1f(uUseSpecularMapLocation, 0.0f); // Disable specular map usage
                    }
                }

                // Draw transparent object
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
                glBindVertexArray(0);

                // (Optional) unbind textures afterwards
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, 0);
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, 0);
                glActiveTexture(GL_TEXTURE3);
                glBindTexture(GL_TEXTURE_2D, 0);
            }

            // Restore depth writing
            glDepthMask(GL_TRUE);
            glDisable(GL_BLEND);

            // Reset alpha to default if needed
            if (uAlphaLocation != -1)
            {
                glUniform1f(uAlphaLocation, 1.0f);
            }
        }


        // for (const auto &object : utils_scene::sceneObjects) {
        //     std::cout << "Object: " << object.name << ", Material Index: " << object.materialIndex << std::endl;
        //     const Material& mat = materialManager.getMaterial(object.materialIndex);
        //     std::cout << "  Has Diffuse Map: " << mat.hasDiffuseMap
        //             << ", Diffuse Map ID: " << mat.diffuseMapID
        //             << ", Has Normal Map: " << mat.hasNormalMap
        //             << ", Normal Map ID: " << mat.normalMapID << std::endl;
        // }

        // Re-enable face culling after rendering transparent objects
        // glEnable(GL_CULL_FACE);

        // Swap buffers
        windowManager.swapBuffers();
    }

    // Clean up resources
    glDeleteBuffers(1, &sphereVBO);
    glDeleteVertexArrays(1, &sphereVAO);

    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);
    glDeleteVertexArrays(1, &cubeVAO);

    // At the end of main, before program termination, we clean up textures from the vector allTextures from file models.cpp
    for (const auto &texture : utils_object::allTextures)
    {
        // print the texture name
        std::cout << "Deleting model texture: " << texture << std::endl;
        glDeleteTextures(1, &texture);
    }

    for (const auto &texture : utils_loader::allTextures)
    {
        // print the texture name
        std::cout << "Deleting textures: " << texture << std::endl;
        glDeleteTextures(1, &texture);
    }

    // Clean up model buffers
    // glDeleteBuffers(1, &heaterModelData.vbo);
    // glDeleteBuffers(1, &heaterModelData.ebo);
    // glDeleteVertexArrays(1, &heaterModelData.vao);

    glDeleteBuffers(1, &rockingChairModelData.vbo);
    glDeleteBuffers(1, &rockingChairModelData.ebo);
    glDeleteVertexArrays(1, &rockingChairModelData.vao);

    glDeleteBuffers(1, &torusModelData.vbo);
    glDeleteBuffers(1, &torusModelData.ebo);
    glDeleteVertexArrays(1, &torusModelData.vao);

    // Clean up framebuffer and texture
    glDeleteFramebuffers(1, &shadowMapFBO);
    glDeleteTextures(1, &depthCubeMap);

    // Clean up shaders
    // depthShader.deleteProgram();
    // room1.deleteProgram();
    // room2.deleteProgram();
    // skyboxShader.deleteProgram();

    // Clean up materials
    // materialManager.clear();

    // Clean up scene objects
    utils_scene::sceneObjects.clear();
    utils_scene::sceneObjectsTransparent.clear();
    utils_scene::sceneObjectsSkybox.clear();

    // Clean up simple lights
    simpleLights.clear();

    // other cleanup
    utils_object::allTextures.clear();
    utils_loader::allTextures.clear();

    std::cout << "Program terminated successfully" << std::endl;

    return 0;
}