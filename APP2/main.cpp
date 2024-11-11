#include "global.hpp"
#include "cube.hpp"
#include "sphere.hpp"
#include "scene_object.hpp"
#include "texture.hpp"
#include "collision.hpp"
#include "utilities.hpp"
#include "resource_loader.hpp"

#include <glad/glad.h>
#include <iostream>
#include <glimac/Program.hpp>
#include <glimac/Image.hpp>
#include <glimac/SDLWindowManager.hpp>
#include <SDL2/SDL.h>
#include <cstddef> // For offsetof
#include <vector>
#include <map>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    std::cout << "Program started" << std::endl;

    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Set OpenGL context version and profile
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) != 0)
        std::cerr << "Failed to set SDL_GL_CONTEXT_MAJOR_VERSION: " << SDL_GetError() << std::endl;
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0)
        std::cerr << "Failed to set SDL_GL_CONTEXT_MINOR_VERSION: " << SDL_GetError() << std::endl;
    if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0)
        std::cerr << "Failed to set SDL_GL_CONTEXT_PROFILE_MASK: " << SDL_GetError() << std::endl;

    #ifdef __APPLE__
        if (SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG) != 0)
            std::cerr << "Failed to set SDL_GL_CONTEXT_FLAGS: " << SDL_GetError() << std::endl;
    #endif

    // Initialize SDLWindowManager
    glimac::SDLWindowManager windowManager(window_width, window_height, "Boules");
    std::cout << "SDLWindowManager initialized" << std::endl;

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    std::cout << "GLAD initialized" << std::endl;

    // Output OpenGL version and renderer
    const GLubyte *renderer = glGetString(GL_RENDERER);
    const GLubyte *version = glGetString(GL_VERSION);
    if (renderer)
        std::cout << "Renderer: " << renderer << std::endl;
    else
        std::cout << "Renderer is NULL" << std::endl;

    if (version)
        std::cout << "OpenGL version supported: " << version << std::endl;
    else
        std::cout << "OpenGL version is NULL" << std::endl;

    // Check if glGenVertexArrays is available
    if (glGenVertexArrays == nullptr)
    {
        std::cerr << "Error: glGenVertexArrays is NULL" << std::endl;
        return -1;
    }

    /*********************************
     * Initialization code
     *********************************/

    // Sphere creation
    glimac::Sphere sphere(1, 32, 16);
    std::vector<SphereVertex> sphereVertices;

    size_t sphereVertexCount = createSphereVertices(sphereVertices, sphere);
    computeSphereTangents(sphereVertices);

    GLuint sphereVBO, sphereVAO;
    setupSphereBuffers(sphereVertices, sphereVBO, sphereVAO);

    GLsizei sphereVertexCountGL = static_cast<GLsizei>(sphereVertexCount);

    // Cube setup
    // Create cube vertices and indices
    std::vector<Vertex3D> cubeVertices;
    std::vector<GLuint> cubeIndices;
    createCube(cubeVertices, cubeIndices);
    computeTangents(cubeVertices, cubeIndices);

    // Set up VBO, EBO, and VAO for the cube
    GLuint cubeVBO, cubeEBO, cubeVAO;
    setupCubeBuffers(cubeVertices, cubeIndices, cubeVBO, cubeEBO, cubeVAO);
    std::cout << "Cube VBO, EBO, and VAO set up" << std::endl;

    // Load shaders
    glimac::FilePath applicationPath(argv[0]);

    // Load unified shader program
    glimac::Program unifiedProgram, depthProgram;
    unifiedProgram = loadUnifiedShader(applicationPath);
    depthProgram = loadDepthShader(applicationPath);

    GLuint textureID, stoneTextureID, brownTerracottaTextureID, soccerTextureID;
    GLuint textureID_normalMap, stoneTextureID_normalMap, brownTerracottaTextureID_normalMap, soccerTextureID_normalMap;
    loadTextures(textureID, stoneTextureID, brownTerracottaTextureID, soccerTextureID,
                textureID_normalMap, stoneTextureID_normalMap, brownTerracottaTextureID_normalMap, soccerTextureID_normalMap, applicationPath);

    GLuint depthCubeMap, shadowMapFBO;
    setupDepthCubeMap(depthCubeMap, shadowMapFBO);

    // Check depth shader uniforms
    std::cout << "Checking depth shader uniforms..." << std::endl;
    GLint uDepth_LightSpaceMatrixLocation = glGetUniformLocation(depthProgram.getGLId(), "uLightSpaceMatrix");
    GLint uDepth_ModelMatrixLocation = glGetUniformLocation(depthProgram.getGLId(), "uModelMatrix");
    if (uDepth_LightSpaceMatrixLocation == -1)
        std::cerr << "Failed to get 'uLightSpaceMatrix' location in depth shader" << std::endl;
    if (uDepth_ModelMatrixLocation == -1)
        std::cerr << "Failed to get 'uModelMatrix' location in depth shader" << std::endl;

    // Get uniform location for model matrix in unified shader
    GLint uModelMatrixLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uModelMatrix");
    if (uModelMatrixLocation == -1)
        std::cerr << "Failed to get 'uModelMatrix' location in unified shader" << std::endl;

    // Get uniform locations
    unifiedProgram.use();
    std::cout << "Shader program in use" << std::endl;

    // Get uniform locations
    GLint uMVPMatrixLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uMVPMatrix");
    GLint uMVMatrixLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uMVMatrix");
    GLint uNormalMatrixLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uNormalMatrix");
    GLint uTextureLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uTexture");
    GLint uUseTextureLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uUseTexture");

    GLint uKdLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uKd");
    GLint uKsLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uKs");
    GLint uShininessLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uShininess");
    GLint uLightPos_vsLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uLightPos_vs");
    GLint uLightIntensityLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uLightIntensity");

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

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    std::cout << "Depth test enabled" << std::endl;

    // Set the clear color to a dark gray
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Light properties
    float spiralRadius = 3.0f; // Radius of the spiral
    float spiralSpeed = 1.0f;   // Speed of the spiral movement
    float lightHeight = 5.0f;   // Height variation of the light

    // Light space matrix for shadows
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.0f);
    glm::mat4 lightView = glm::lookAt(glm::vec3(5.0f, 10.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    // Setup projection matrix for cube map
    float nearPlane = 1.0f;
    float farPlane = 25.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.0f, nearPlane, farPlane);

    // Camera parameters
    glm::vec3 cameraPos(0.0f, 0.0f, 5.0f);    // Initial position of the camera
    glm::vec3 cameraFront(0.0f, 0.0f, -1.0f); // Direction the camera is looking at
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);     // Up vector

    float cameraPosY = cameraPos.y; // Store the initial y position

    float yaw = -90.0f; // Yaw angle initialized to -90 degrees to look towards negative Z
    float pitch = 0.0f; // Pitch angle

    float cameraSpeed = 5.0f; // Adjust accordingly
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

    // Add sphere to the scene
    addSphere(
        glm::vec3(0.0f, 0.0f, 0.0f), // Position
        1.0f,                        // Radius
        glm::vec3(1.0f),             // Color (white)
        true,                        // Use texture
        soccerTextureID,             // Texture ID
        soccerTextureID_normalMap,   // Normal map ID
        sphereVAO,                   // VAO ID
        sphereVertexCount            // Vertex count
    );

    addCube(
        glm::vec3(-2.0f, -1.0f, 0.0f), // Position
        glm::vec3(1.0f),              // Scale
        glm::vec3(0.2f, 1.0f, 0.1f),  // Color
        false,                        // Use texture
        0,                            // Texture ID
        0,                            // Normal map ID
        glm::vec3(0.0f),              // Rotation axis
        0.0f,                         // Rotation angle
        cubeVAO,                      // VAO ID
        cubeIndexCount                // Index count
    );

    addCube(
        glm::vec3(0.0f, -1.0f, 0.0f), // Position
        glm::vec3(0.5f, 1.0f, 0.5f), // Scale (non-uniform)
        glm::vec3(0.3f),             // Color (gray)
        false,                       // Use texture
        0,                           // Texture ID
        0,                           // Normal map ID
        glm::vec3(1.0f),             // Rotation axis
        0.0f,                        // Rotation angle
        cubeVAO,                     // VAO ID
        cubeIndexCount               // Index count
    );

    // Add textured cube
    addCube(
        glm::vec3(0.0f, 0.0f, 2.0f), // Position
        glm::vec3(1.0f),             // Scale
        glm::vec3(1.0f),             // Color (white, not used)
        true,                        // Use texture
        textureID,                   // Texture ID
        textureID_normalMap,         // Normal map ID
        glm::vec3(0.0f),             // Rotation axis
        0.0f,                        // Rotation angle
        cubeVAO,                     // VAO ID
        cubeIndexCount               // Index count
    );

    // Add textured cube
    addCube(
        glm::vec3(0.0f, -1.0f, 3.0f), // Position
        glm::vec3(1.0f),             // Scale
        glm::vec3(1.0f),             // Color (white, not used)
        true,                        // Use texture
        brownTerracottaTextureID,    // Texture ID
        brownTerracottaTextureID_normalMap,
        glm::vec3(0.0f),             // Rotation axis
        0.0f,                        // Rotation angle
        cubeVAO,                     // VAO ID
        cubeIndexCount               // Index count
    );

    // Add floor grid cubes
    for (int x = 0; x < numCubesX; ++x)
    {
        for (int z = 0; z < numCubesZ; ++z)
        {
            glm::vec3 position(-7.0f + x * spacingX, -2.0f, 7.0f - z * spacingZ);
            addCube(
                position,        // Position
                glm::vec3(1.0f), // Scale
                glm::vec3(1.0f), // Color (white, not used)
                true,            // Use texture
                stoneTextureID,  // Texture ID
                stoneTextureID_normalMap,
                glm::vec3(0.0f), // Rotation axis
                0.0f,            // Rotation angle
                cubeVAO,         // VAO ID
                cubeIndexCount   // Index count
            );
        }
    }

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

            // Update window title with FPS
            std::string newTitle = "Boules - FPS: " + std::to_string(fps);
            SDL_SetWindowTitle(windowManager.getWindow(), newTitle.c_str());
        }

        // Update light intensity dynamically within the loop
        glm::vec3 lightIntensity = glm::vec3(
            (sin(currentFrame) + 1.0f) / 2.0f,       // Red oscillates between 0 and 1
            (cos(currentFrame) + 1.0f) / 2.0f,       // Green oscillates between 0 and 1
            (sin(currentFrame * 0.5f) + 1.0f) / 2.0f // Blue oscillates more slowly between 0 and 1
        );
        // white light
        // glm::vec3 lightIntensity = glm::vec3(1.0f);

        // Event handling
        SDL_Event e;
        while (windowManager.pollEvent(e))
        {
            if (e.type == SDL_QUIT)
            {
                done = true;
            }
            if (e.type == SDL_KEYDOWN)
            {
                if (e.key.keysym.sym == SDLK_ESCAPE)
                {
                    done = true;
                }
            }

            // Mouse movement
            if (e.type == SDL_MOUSEMOTION)
            {
                float xpos = e.motion.xrel;
                float ypos = e.motion.yrel;

                float sensitivity = 0.1f; // Adjust this value
                xpos *= sensitivity;
                ypos *= sensitivity;

                yaw += xpos;
                pitch -= ypos; // Invert y-axis if necessary

                // Constrain pitch
                if (pitch > 89.0f)
                    pitch = 89.0f;
                if (pitch < -89.0f)
                    pitch = -89.0f;

                // Update camera front vector
                glm::vec3 front;
                front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                front.y = sin(glm::radians(pitch));
                front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront = glm::normalize(front);
            }
        }

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
        for (const auto &object : sceneObjects)
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

        // Update light position to move in a circle at a fixed height
        float spiralRadius = 2.0f;    // Adjust the radius of the circle
        float spiralSpeed = 0.5f;      // Adjust the speed of the rotation
        float fixedHeight = 2.0f;      // Set the fixed height of the light

        glm::vec3 lightPosWorld;
        lightPosWorld.x = spiralRadius * cos(currentFrame * spiralSpeed);
        lightPosWorld.y = fixedHeight; // Fixed height
        lightPosWorld.z = spiralRadius * sin(currentFrame * spiralSpeed);

        // fixed light position
        // glm::vec3 lightPosWorld = glm::vec3(2.0f, 0.6f, 2.0f);

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
        glViewport(0, 0, 8192, 8192); // Match shadow map resolution
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Use the depth shader program
        depthProgram.use();
        glUniform1f(glGetUniformLocation(depthProgram.getGLId(), "farPlane"), farPlane);
        glUniform3fv(glGetUniformLocation(depthProgram.getGLId(), "lightPos"), 1, glm::value_ptr(lightPosWorld));

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
            depthProgram.use();
            glUniform1f(glGetUniformLocation(depthProgram.getGLId(), "farPlane"), farPlane);
            glUniform3fv(glGetUniformLocation(depthProgram.getGLId(), "lightPos"), 1, glm::value_ptr(lightPosWorld));

            // Set the shadow matrix for the current face
            glUniformMatrix4fv(glGetUniformLocation(depthProgram.getGLId(), "shadowMatrix"), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));

            // Render scene objects
            for (const auto &object : sceneObjects) {
                glm::mat4 modelMatrix = glm::mat4(1.0f);
                modelMatrix = glm::translate(modelMatrix, object.position);
                if (object.rotationAngle != 0.0f) {
                    modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotationAngle), object.rotationAxis);
                }
                modelMatrix = glm::scale(modelMatrix, object.scale);

                // Set model matrix for depth shader
                glUniformMatrix4fv(glGetUniformLocation(depthProgram.getGLId(), "model"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

                glBindVertexArray(object.vaoID);
                if (object.type == ObjectType::Cube) {
                    glDrawElements(GL_TRIANGLES, object.indexCount, GL_UNSIGNED_INT, 0);
                } else if (object.type == ObjectType::Sphere) {
                    glDrawArrays(GL_TRIANGLES, 0, object.indexCount);
                }
                glBindVertexArray(0);
            }

            // Optional: Unbind the framebuffer after each face
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // Second Pass: Render the scene normally with point light
        glViewport(0, 0, window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        unifiedProgram.use();

        // Set light properties
        glUniform3fv(uLightPos_vsLocation, 1, glm::value_ptr(lightPosViewSpace));
        glUniform3fv(uLightIntensityLocation, 1, glm::value_ptr(lightIntensity));

        // Set the updated light space matrix
        glUniformMatrix4fv(glGetUniformLocation(unifiedProgram.getGLId(), "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // Bind the depth cube map to texture unit 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
        glUniform1i(glGetUniformLocation(unifiedProgram.getGLId(), "depthMap"), 1);

        // Set light position in world space
        glUniform3fv(glGetUniformLocation(unifiedProgram.getGLId(), "lightPosWorld"), 1, glm::value_ptr(lightPosWorld));

        // Set camera position in world space
        glUniform3fv(glGetUniformLocation(unifiedProgram.getGLId(), "cameraPosWorld"), 1, glm::value_ptr(cameraPos));

        // Set far plane
        glUniform1f(glGetUniformLocation(unifiedProgram.getGLId(), "farPlane"), farPlane);

        // Render all scene objects
        for (const auto &object : sceneObjects)
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
                glUniform1i(glGetUniformLocation(unifiedProgram.getGLId(), "uNormalMap"), 2);
                glUniform1f(glGetUniformLocation(unifiedProgram.getGLId(), "uUseNormalMap"), 1.0f);
            }
            else
            {
                glUniform1f(glGetUniformLocation(unifiedProgram.getGLId(), "uUseNormalMap"), 0.0f); // Disable normal map usage
            }

            glBindVertexArray(object.vaoID);
            if (object.type == ObjectType::Cube)
            {
                glDrawElements(GL_TRIANGLES, object.indexCount, GL_UNSIGNED_INT, 0);
            }
            else if (object.type == ObjectType::Sphere)
            {
                glDrawArrays(GL_TRIANGLES, 0, object.indexCount);
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
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2f)); // Adjust the size as needed

            // Calculate matrices
            glm::mat4 mvMatrix = ViewMatrix * modelMatrix;
            glm::mat4 mvpMatrix = ProjMatrix * mvMatrix;
            glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(mvMatrix)));

            // Use the unified shader program
            unifiedProgram.use();

            // **Set transformation matrices**
            glUniformMatrix4fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));
            glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
            glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
            glUniformMatrix3fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

            // **Set camera position in world space (for lighting calculations)**
            glUniform3fv(glGetUniformLocation(unifiedProgram.getGLId(), "cameraPosWorld"), 1, glm::value_ptr(cameraPos));

            // **Set light position in world space (even if not needed for the light sphere)**
            glUniform3fv(glGetUniformLocation(unifiedProgram.getGLId(), "lightPosWorld"), 1, glm::value_ptr(lightPosWorld));

            // **Set light properties**
            glUniform3fv(uLightPos_vsLocation, 1, glm::value_ptr(lightPosViewSpace));
            glUniform3fv(uLightIntensityLocation, 1, glm::value_ptr(lightIntensity));

            // **Set far plane**
            glUniform1f(glGetUniformLocation(unifiedProgram.getGLId(), "farPlane"), farPlane);

            // Bind depth cube map
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
            glUniform1i(glGetUniformLocation(unifiedProgram.getGLId(), "depthMap"), 1);

            // Set material properties (emissive)
            glm::vec3 Kd = lightIntensity / 5.0f; // Adjust as needed
            glm::vec3 Ks = glm::vec3(0.0f);       // No specular for the light indicator
            float shininess = 0.0f;
            glUniform3fv(uKdLocation, 1, glm::value_ptr(Kd));
            glUniform3fv(uKsLocation, 1, glm::value_ptr(Ks));
            glUniform1f(uShininessLocation, shininess);

            // Disable textures
            glUniform1f(uUseTextureLocation, 0.0f);
            glUniform1f(glGetUniformLocation(unifiedProgram.getGLId(), "uUseNormalMap"), 0.0f);

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

    std::cout << "Program terminated successfully" << std::endl;

    return 0;
}