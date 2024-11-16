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
    glimac::Program room_1Program, room_2Program, depthProgram;

    // Load shaders
    room_1Program = loadRoom1Shader(applicationPath);
    room_2Program = loadRoom2Shader(applicationPath);
    depthProgram = loadDepthShader(applicationPath);


    // ========================================
    // Resources setup END
    // ========================================


    // ========================================
    // Lights setup START
    // ========================================
    std::cout << "Setting up lights" << std::endl;

    // light 1

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
    glm::vec3 cameraPos(1.0f, 2.0f, 1.0f);    // Initial position of the camera
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

    // =============== FPS variables ===============
    int frameCount = 0;
    float fpsTimer = 0.0f;
    int fps = 0;

    // ============ relative mosue mvmt ============
    SDL_SetRelativeMouseMode(SDL_TRUE);


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
                    object.position = object.initialPosition + offsetPosition;

                    object.rotationAngle = rotation.z;                 // Rotation around Z-axis
                    object.rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f); // Rotation around X-axis

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


        // ========================================
        // First pass: Culling END
        // ========================================

        // ========================================
        // Second pass: Shadow mapping START
        // ========================================
        // render the scene from all lights' point of view


        // ========================================
        // Second pass: Shadow mapping END
        // ========================================


        // ========================================
        // Third pass: G buffer rendering START
        // ========================================

        // ========================================
        // Third pass: G buffer rendering END
        // ========================================

        // unbind framebuffer to render to default framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ========================================
        // Fourth pass: Lighting START
        // ========================================

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
    Graphics::deleteProgram(room_1Program.getGLId());
    Graphics::deleteProgram(room_2Program.getGLId());
    Graphics::deleteProgram(depthProgram.getGLId());

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

}





