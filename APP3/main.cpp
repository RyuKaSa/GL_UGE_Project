// includes from files
#include "global.hpp"
#include "models.hpp"
#include "point_light.hpp"
#include "resource_loader.hpp"
#include "scene_objects.hpp"
#include "graphics.hpp"


// includes from libraries
#include <iostream>
#include <glad/glad.h>
#include <glimac/SDLWindowManager.hpp>
#include <SDL2/SDL.h>

// window dimensions
int window_width = 1600;
int window_height = 1000;

void renderScreenQuad() {
    static unsigned int quadVAO = 0;
    static unsigned int quadVBO;

    // Create VAO/VBO if not already done
    if (quadVAO == 0) {
        float quadVertices[] = {
            // Positions   // Texture Coords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
        };

        // Generate and bind VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);

        // Fill buffer data
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

        // Position attribute
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        // Texture coordinate attribute
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    }

    // Render the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

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

    // Enable depth testing and face culling
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);    

    Graphics::GBuffer gBuffer;
    int gBufferWidth = 1024;
    int gBufferHeight = 1024;

    if (!gBuffer.initialize(gBufferWidth, gBufferHeight))
    {
        std::cerr << "Failed to initialize GBuffer" << std::endl;
        return -1;
    }

    /*********************************
     * Initialization code
     *********************************/

    // ========================================
    // Resources setup START
    // ========================================
    std::cout << "Setting up resources + shaders + textures + normals" << std::endl;
    glimac::FilePath applicationPath(argv[0]);

    // textures init
    GLuint brownTerracottaTextureID, brownTerracottaTextureID_normalMap;
    GLuint chairBaseColorTextureID, chairNormalMapTextureID;

    // Load textures
    loadTextures(brownTerracottaTextureID,
                 brownTerracottaTextureID_normalMap,
                 chairBaseColorTextureID,
                 chairNormalMapTextureID,
                 applicationPath);

    // shaders init
    glimac::Program room_1Program, room_2Program, depthProgram, lightingProgram;

    // Load shaders
    room_1Program = loadRoom_1Shader(applicationPath);
    room_2Program = loadRoom_2Shader(applicationPath);
    depthProgram = loadDepthShader(applicationPath);
    lightingProgram = loadLightingShader(applicationPath);

    // check
    if (room_1Program.getGLId() == 0 || room_2Program.getGLId() == 0 || depthProgram.getGLId() == 0)
    {
        std::cerr << "Failed to load shaders" << std::endl;
        return -1;
    }


    // ========================================
    // Resources setup END
    // ========================================


    // ========================================
    // Lights setup START
    // ========================================
    std::cout << "Setting up lights" << std::endl;

    // light 1
    Point_Light::PointLight pointLight1;
    pointLight1.position = glm::vec3(0.0f, 4.0f, 0.0f);
    pointLight1.color = glm::vec3(1.0f, 1.0f, 1.0f);
    pointLight1.intensity = 1.0f;
    pointLight1.direction = glm::vec3(0.0f, -1.0f, 0.0f);

    // light 2


    // ========================================
    // Lights setup END
    // ========================================

    // ========================================
    // Objects setup START
    // ========================================
    std::cout << "Setting up scene objects" << std::endl;

    // ============ cubes ============

    // =========== spheres ===========

    // =========== imports ===========

    // load the Rocking chair, for the animated object
    ModelData rockingChairModelData;
    std::string rockingChairPath = applicationPath.dirPath() + "assets/models/Rocking_Chair/kid_rocking_chair.obj";
    std::string rockingChairBasePath = applicationPath.dirPath() + "assets/models/Rocking_Chair/Textures/";

    if (!ModelUtils::loadOBJ(rockingChairPath, rockingChairBasePath, rockingChairModelData))
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
    GLuint baseColorTextureID = ModelUtils::LoadTextureFromFile(baseColorPath.c_str());
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
    GLuint normalMapTextureID = ModelUtils::LoadTextureFromFile(normalMapPath.c_str());
    if (normalMapTextureID == 0)
    {
        std::cerr << "Failed to load Normal.png for Rocking Chair." << std::endl;
    }
    else
    {
        std::cout << "Normal.png for Rocking Chair loaded successfully: ID " << normalMapTextureID << std::endl;
    }

    // Set up OpenGL buffers for the Rocking Chair model
    ModelUtils::setupModelBuffers(rockingChairModelData);

    // Compute Bounding Box for the Rocking Chair model
    AABB rockingChairModelBoundingBox = computeAABB(rockingChairModelData.vertices);

    // Apply scale
    glm::vec3 rockingChairModelScale(0.8f, 0.8f, 0.8f);
    rockingChairModelBoundingBox.min *= rockingChairModelScale;
    rockingChairModelBoundingBox.max *= rockingChairModelScale;

    // Apply translation (position)
    glm::vec3 rockingChairModelPosition(5.0f, 0.56f, 9.0f);
    rockingChairModelBoundingBox.min += rockingChairModelPosition;
    rockingChairModelBoundingBox.max += rockingChairModelPosition;

    // Add Rocking Chair Model to Scene Objects
    SceneObjectManager::addModel(
        "rocking_chair",                                            // Name
        rockingChairModelPosition,                                  // Position
        rockingChairModelScale,                                     // Scale
        true,                                                       // Use texture
        baseColorTextureID,                                         // Texture ID (Base Color)
        normalMapTextureID,                                         // Normal Map ID
        rockingChairModelData.vao,                                  // VAO ID
        static_cast<GLsizei>(rockingChairModelData.indices.size()), // Index Count
        rockingChairModelBoundingBox,                               // Bounding Box
        glm::vec3(0.0f, 0.0f, 0.0f),                                // Rotation Axis (Y-axis)
        0.0f,                                                       // Rotation Angle
        false
    );

    // rocking chair parameters
    double frequency = 0.30; // Rocking frequency (cycles per second)
    double length = 0.08;    // Maximum angle in degrees
    double radius = 0.3;     // Radius of the rocking base

    // ========================================
    // Objects setup END
    // ========================================

    // ========================================
    // Main loop variables START
    // ========================================

    // scene objects management :
    //  - one for ALL scene objects (all are pushed into this one at creation)
    //  - one for the dynamic objects
    //  - one for the visible objects

    // the vectors are already initialized in the scene_objects.cpp file
    // Populate dynamicObjects with pointers to objects in allObjects where isStatic == false
    for (SceneObjectManager::SceneObject& obj : SceneObjectManager::allObjects) {
        if (!obj.isStatic) {
            SceneObjectManager::dynamicObjects.push_back(&obj); // Store a pointer to the non-static object
        }
    }
    std::cout << "Dynamic objects: " << SceneObjectManager::dynamicObjects.size() << std::endl;
    std::cout << "For a total of :" << SceneObjectManager::allObjects.size() << std::endl;

    // ======== animation utility variables ========
    bool isRockingChairPaused = false;
    double rockingChairStartTime = windowManager.getTime();
    double rockingChairPausedTime;

    // ============= Camera parameters =============
    glm::vec3 cameraPos(0.0f, 2.0f, -1.0f);    // Initial position of the camera
    glm::vec3 cameraFront(0.0f, 0.0f, -1.0f); // Direction the camera is looking at
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);     // Up vector

    float cameraPosY = cameraPos.y; // Store the initial y position

    float yaw = -90.0f; // Yaw angle initialized to -90 degrees to look towards negative Z
    float pitch = 0.0f; // Pitch angle

    float cameraSpeed = 5.0f; // Adjust accordingly
    float deltaTime = 0.0f;   // Time between current frame and last frame
    float lastFrame = 0.0f;   // Time of last frame

    // Camera collision parameters
    float cameraRadius = 0.3f; // Radius of the camera for collision detection
    float cameraHeight = 1.8f; // Height of the camera cylinder

    glm::mat4 viewMatrix = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), (float)window_width / (float)window_height, 0.1f, 100.0f);

    // =============== FPS variables ===============
    int frameCount = 0;
    float fpsTimer = 0.0f;
    int fps = 0;

    // ============ relative mosue mvmt ============
    SDL_SetRelativeMouseMode(SDL_TRUE);


    // ============= shadow parameters =============
    const GLuint SHADOW_WIDTH = 1024;
    const GLuint SHADOW_HEIGHT = 1024;

    // ========================================
    // Main loop variables END
    // ========================================



    /*********************************
     * Main loop
     *********************************/

    // Main loop variables
    bool done = false;
    std::cout << "Entering main loop" << std::endl;

    while (!done)
    {

        // ========================================
        // FPS Counter START
        // ========================================

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

        // update window title with FPS
        std::string newTitle = "FPS: " + std::to_string(fps);
        SDL_SetWindowTitle(windowManager.getWindow(), newTitle.c_str());

        // ========================================
        // FPS Counter END
        // ========================================


        // ========================================
        // Event handling + collisions START
        // ========================================

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
                else if (e.key.keysym.sym == SDLK_t)
                {
                    isRockingChairPaused = !isRockingChairPaused;
                    if (isRockingChairPaused)
                    {
                        // Record the time when paused
                        rockingChairPausedTime = currentFrame;
                    }
                    else
                    {
                        // Adjust the start time when unpausing
                        double pauseDuration = currentFrame - rockingChairPausedTime;
                        rockingChairStartTime += pauseDuration;
                    }
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
        for (const auto &object : SceneObjectManager::allObjects)
        
        {
            if (SceneObjectManager::checkCollision(proposedCameraPos, cameraRadius, cameraHeight, object.boundingBox))
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


        // ========================================
        // Event handling + collisions END
        // ========================================


        // ========================================
        // Update dynamic objects START
        // ========================================

        for (SceneObjectManager::SceneObject* obj : SceneObjectManager::dynamicObjects) {
            // Update the rocking chair
            if (obj->name == "rocking_chair") {
                if (!isRockingChairPaused) {

                    double adjustedTime = currentFrame - rockingChairStartTime;

                    if (isRockingChairPaused)
                    {
                        adjustedTime = rockingChairPausedTime - rockingChairStartTime;
                    }

                    glm::vec3 offsetPosition;
                    glm::vec3 rotation;
                    ModelUtils::GetRockingChairPositionAndRotation(
                        adjustedTime,
                        frequency,
                        radius,
                        length,
                        offsetPosition,
                        rotation);

                    // Update position and rotation
                    obj->position = obj->initialPosition + offsetPosition;
                    obj->rotationAngle = rotation.z;
                    obj->rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f);

                    // in this case, we won't update the boundign box,
                    // easier to handle because we know the scope of the animation
                
                }
            }
        }

        // ========================================
        // Update dynamic objects END
        // ========================================


        /*********************************
         * Rendering code START
         *********************************/

        // clear buffers
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer.fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ========================================
        // First pass: Culling START
        // ========================================
        // camera perspective culling, into a list of visible objects to render
        // go through allObjects, and push references to visibleObjects if they are in the camera view

        for (SceneObjectManager::SceneObject& obj : SceneObjectManager::allObjects) {
            // Check if the object's bounding box is in the frustum
            if (SceneObjectManager::isBoxInFrustum(obj.boundingBox, obj.position, obj.scale)) {
                SceneObjectManager::visibleObjects.push_back(&obj);
            }
        }

        // ========================================
        // First pass: Culling END
        // ========================================

        // ========================================
        // Second pass: Shadow mapping START
        // ========================================
        // render the scene from all lights' point of view

        // Shadow mapping pass
        if (pointLight1.shadowMapFBO == 0) {
            // Generate shadow map framebuffer if not already done
            glGenFramebuffers(1, &pointLight1.shadowMapFBO);

            // Create depth texture for shadow mapping
            glGenTextures(1, &pointLight1.shadowMap);
            glBindTexture(GL_TEXTURE_2D, pointLight1.shadowMap);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

            // Attach depth texture as FBO's depth buffer
            glBindFramebuffer(GL_FRAMEBUFFER, pointLight1.shadowMapFBO);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, pointLight1.shadowMap, 0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                std::cerr << "Shadow Framebuffer not complete!" << std::endl;
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // Set up the shadow shader and matrices
        depthProgram.use();

        // Compute light matrices
        float near_plane = 1.0f, far_plane = 25.0f;
        glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(pointLight1.position, pointLight1.position + pointLight1.direction, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        // Get uniform location for lightSpaceMatrix in the depth shader program
        GLint lightSpaceMatrixLoc = glGetUniformLocation(depthProgram.getGLId(), "lightSpaceMatrix");
        if (lightSpaceMatrixLoc != -1) {
            glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        }

        // Render objects to create the shadow map
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, pointLight1.shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        for (SceneObjectManager::SceneObject& obj : SceneObjectManager::allObjects) { // Render all objects for shadow mapping
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, obj.position);
            model = glm::rotate(model, glm::radians(obj.rotationAngle), obj.rotationAxis);
            model = glm::scale(model, obj.scale);

            // Get uniform location for model matrix in the depth shader program
            GLint modelLoc = glGetUniformLocation(depthProgram.getGLId(), "model");
            if (modelLoc != -1) {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            }

            // Bind the object's VAO and render
            glBindVertexArray(obj.vaoID);
            glDrawElements(GL_TRIANGLES, obj.indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Restore viewport
        glViewport(0, 0, gBufferWidth, gBufferHeight);


        // ========================================
        // Second pass: Shadow mapping END
        // ========================================


        // ========================================
        // Third pass: G buffer rendering START
        // ========================================

        // Activate the shader program for rendering the scene
        room_1Program.use(); 

        // Set the view matrix uniform
        GLint viewLoc = glGetUniformLocation(room_1Program.getGLId(), "view");
        if (viewLoc != -1) {
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
        }

        // Set the projection matrix uniform
        GLint projLoc = glGetUniformLocation(room_1Program.getGLId(), "projection");
        if (projLoc != -1) {
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
        }

        // Set the light position uniform
        GLint lightPosLocRoom = glGetUniformLocation(room_1Program.getGLId(), "lightPos");
        if (lightPosLocRoom != -1) {
            glUniform3fv(lightPosLocRoom, 1, glm::value_ptr(pointLight1.position));
        }

        // Set the light color uniform
        GLint lightColorLocRoom = glGetUniformLocation(room_1Program.getGLId(), "lightColor");
        if (lightColorLocRoom != -1) {
            glUniform3fv(lightColorLocRoom, 1, glm::value_ptr(pointLight1.color));
        }

        // Set the light intensity uniform
        GLint lightIntensityLocRoom = glGetUniformLocation(room_1Program.getGLId(), "lightIntensity");
        if (lightIntensityLocRoom != -1) {
            glUniform1f(lightIntensityLocRoom, pointLight1.intensity);
        }

        // Set the camera/view position uniform
        GLint viewPosLocRoom = glGetUniformLocation(room_1Program.getGLId(), "viewPos");
        if (viewPosLocRoom != -1) {
            glUniform3fv(viewPosLocRoom, 1, glm::value_ptr(cameraPos));
        }

        // Bind shadow map and set shadow map sampler uniform
        glActiveTexture(GL_TEXTURE1); // Bind shadow map to texture unit 1
        glBindTexture(GL_TEXTURE_2D, pointLight1.shadowMap);
        GLint shadowMapLocRoom = glGetUniformLocation(room_1Program.getGLId(), "shadowMap");
        if (shadowMapLocRoom != -1) {
            glUniform1i(shadowMapLocRoom, 1); // Set the shadow map sampler to texture unit 1
        }

        // Render all visible objects
        for (SceneObjectManager::SceneObject* obj : SceneObjectManager::visibleObjects) {
            // Set the model matrix for transforming this object
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, obj->position);
            model = glm::rotate(model, glm::radians(obj->rotationAngle), obj->rotationAxis);
            model = glm::scale(model, obj->scale);
            // Set the model matrix uniform
            GLint modelLoc = glGetUniformLocation(room_1Program.getGLId(), "model");
            if (modelLoc != -1) {
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            }

            // Bind diffuse texture if the object uses one
            if (obj->useTexture) {
                glActiveTexture(GL_TEXTURE0); // Bind to texture unit 0
                glBindTexture(GL_TEXTURE_2D, obj->textureID);
                // Set diffuse texture sampler to use texture unit 0
                GLint textureLoc = glGetUniformLocation(room_1Program.getGLId(), "diffuseTexture");

                // bind a normal map if present
                if (obj->normalMapID) {
                    glActiveTexture(GL_TEXTURE2); // Bind to texture unit 2
                    glBindTexture(GL_TEXTURE_2D, obj->normalMapID);
                    // Set normal map sampler to use texture unit 2
                    GLint normalMapLoc = glGetUniformLocation(room_1Program.getGLId(), "normalMap");
                }
            }

            // Render the object using its VAO and index count
            glBindVertexArray(obj->vaoID);
            glDrawElements(GL_TRIANGLES, obj->indexCount, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
        }

        // ========================================
        // Third pass: G buffer rendering END
        // ========================================

        // unbind framebuffer to render to default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ========================================
        // Fourth pass: Lighting START
        // ========================================

        lightingProgram.use();

        GLuint shaderProgramID = lightingProgram.getGLId(); // Get the program ID

        // Set the camera/view position
        GLint viewPosLocLighting = glGetUniformLocation(shaderProgramID, "viewPos");
        if (viewPosLocLighting != -1) {
            glUniform3fv(viewPosLocLighting, 1, glm::value_ptr(cameraPos));
        }
        glUniform3f(viewPosLocLighting, cameraPos.x, cameraPos.y, cameraPos.z);

        // Pass the light's transformation matrix to the shader
        GLint lightSpaceMatrixLocLighting = glGetUniformLocation(shaderProgramID, "lightSpaceMatrix");
        glUniformMatrix4fv(lightSpaceMatrixLocLighting, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // Bind G-buffer textures (assuming these methods provide valid OpenGL texture IDs)
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gBuffer.getPositionTexture()); // Position texture from G-buffer
        GLint gPositionLoc = glGetUniformLocation(shaderProgramID, "gPosition");
        glUniform1i(gPositionLoc, 0); // Tell the shader that texture unit 0 is gPosition

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gBuffer.getNormalTexture()); // Normal texture from G-buffer
        GLint gNormalLoc = glGetUniformLocation(shaderProgramID, "gNormal");
        glUniform1i(gNormalLoc, 1); // Tell the shader that texture unit 1 is gNormal

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gBuffer.getAlbedoSpecTexture()); // Albedo + Specular texture from G-buffer
        GLint gAlbedoSpecLoc = glGetUniformLocation(shaderProgramID, "gAlbedoSpec");
        glUniform1i(gAlbedoSpecLoc, 2); // Tell the shader that texture unit 2 is gAlbedoSpec

        // Bind shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, pointLight1.shadowMap);
        GLint shadowMapLocLighting = glGetUniformLocation(shaderProgramID, "shadowMap");
        glUniform1i(shadowMapLocLighting, 3); // Tell the shader that texture unit 3 is the shadow map

        // Set light properties
        GLint lightPosLocLighting = glGetUniformLocation(shaderProgramID, "light.Position");
        glUniform3f(lightPosLocLighting, pointLight1.position.x, pointLight1.position.y, pointLight1.position.z);

        GLint lightColorLocLighting = glGetUniformLocation(shaderProgramID, "light.Color");
        glUniform3f(lightColorLocLighting, pointLight1.color.x, pointLight1.color.y, pointLight1.color.z);

        GLint lightIntensityLocLighting = glGetUniformLocation(shaderProgramID, "light.Intensity");
        glUniform1f(lightIntensityLocLighting, pointLight1.intensity);

        // Render a screen-filling quad (assumes you have a function or VAO for this)
        renderScreenQuad(); // This function should render a full-screen quad using your lighting shader

        // ========================================
        // Fourth pass: Lighting END
        // ========================================


        // ========================================
        // Fifth pass: Forward pass START
        // ========================================
        // for transparent objects

        // ========================================
        // Fifth pass: Forward pass END
        // ========================================



        // ========================================
        // Sixth pass: Post-processing START
        // ========================================

        // ========================================
        // Sixth pass: Post-processing END
        // ========================================


        // ========================================
        // no UI, but it would be here
        // ========================================


        // ========================================
        // Swap buffers START
        // ========================================

        windowManager.swapBuffers();

        // ========================================
        // Swap buffers END
        // ========================================

        /*********************************
         * Rendering code END
         *********************************/
    }

    // ========================================
    // Clean up resources START
    // ========================================

    gBuffer.cleanup();

    // Delete textures using Graphics functions
    Graphics::deleteTexture(brownTerracottaTextureID);
    Graphics::deleteTexture(brownTerracottaTextureID_normalMap);
    Graphics::deleteTexture(chairBaseColorTextureID);
    Graphics::deleteTexture(chairNormalMapTextureID);

    // Delete shader programs using Graphics functions
    GLuint room1ID = room_1Program.getGLId();
    Graphics::deleteProgram(room1ID);
    GLuint room2ID = room_2Program.getGLId();
    Graphics::deleteProgram(room2ID);
    GLuint depthID = depthProgram.getGLId();
    Graphics::deleteProgram(depthID);
    GLuint lightingID = lightingProgram.getGLId();
    Graphics::deleteProgram(lightingID);

    // Delete VAOs, VBOs, and EBOs for all scene objects using Graphics functions
    for (SceneObjectManager::SceneObject& obj : SceneObjectManager::allObjects)
    {
        Graphics::deleteSceneObjectBuffers(obj);
    }

    std::cout << "Program terminated successfully" << std::endl;

    // Clean up SDL subsystems
    SDL_Quit();

    // ========================================
    // Clean up resources END
    // ========================================

} // end of main