#define GLFW_INCLUDE_NONE
#include <glad/glad.h>

#include <iostream>
#include <glimac/Sphere.hpp>
#include <glimac/Program.hpp>
#include <glimac/Image.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "glimac/SDLWindowManager.hpp"
#include <SDL2/SDL.h>
#include <cstddef> // For offsetof
#include <vector>

int window_width = 800;
int window_height = 800;

// stone floot coords
int numCubesX = 15; // Number of cubes along the x-axis
int numCubesZ = 15; // Number of cubes along the z-axis
float spacingX = 1.0f; // Distance between each cube along the x-axis
float spacingZ = 1.0f; // Distance between each cube along the z-axis

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

std::vector<AABB> objectBoundaries; // List of all object boundaries
float cameraRadius = 0.15f; // Radius of the camera sphere for collision detection

// Structure for 3D vertices with position, normal, and texture coordinates
struct Vertex3D {
    glm::vec3 position;    // Vertex position
    glm::vec3 normal;      // Vertex normal
    glm::vec2 texCoords;   // Texture coordinates

    Vertex3D(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& uv)
        : position(pos), normal(norm), texCoords(uv) {}
};

// Function to generate cube vertices and indices
void createCube(std::vector<Vertex3D>& vertices, std::vector<GLuint>& indices) {
    // Define the cube vertices
    vertices = {
        // Front face
        Vertex3D(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0,  0,  1), glm::vec2(0.0f, 0.0f)),
        Vertex3D(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0,  0,  1), glm::vec2(1.0f, 0.0f)),
        Vertex3D(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0,  0,  1), glm::vec2(1.0f, 1.0f)),
        Vertex3D(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0,  0,  1), glm::vec2(0.0f, 1.0f)),
        // Back face
        Vertex3D(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0,  0, -1), glm::vec2(1.0f, 0.0f)),
        Vertex3D(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0,  0, -1), glm::vec2(0.0f, 0.0f)),
        Vertex3D(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0,  0, -1), glm::vec2(0.0f, 1.0f)),
        Vertex3D(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0,  0, -1), glm::vec2(1.0f, 1.0f)),
        // Left face
        Vertex3D(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1,  0,  0), glm::vec2(0.0f, 0.0f)),
        Vertex3D(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1,  0,  0), glm::vec2(1.0f, 0.0f)),
        Vertex3D(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1,  0,  0), glm::vec2(1.0f, 1.0f)),
        Vertex3D(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1,  0,  0), glm::vec2(0.0f, 1.0f)),
        // Right face
        Vertex3D(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1,  0,  0), glm::vec2(1.0f, 0.0f)),
        Vertex3D(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1,  0,  0), glm::vec2(0.0f, 0.0f)),
        Vertex3D(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1,  0,  0), glm::vec2(0.0f, 1.0f)),
        Vertex3D(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 1,  0,  0), glm::vec2(1.0f, 1.0f)),
        // Top face
        Vertex3D(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0,  1,  0), glm::vec2(0.0f, 1.0f)),
        Vertex3D(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3( 0,  1,  0), glm::vec2(0.0f, 0.0f)),
        Vertex3D(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 0,  1,  0), glm::vec2(1.0f, 0.0f)),
        Vertex3D(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0,  1,  0), glm::vec2(1.0f, 1.0f)),
        // Bottom face
        Vertex3D(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3( 0, -1,  0), glm::vec2(1.0f, 1.0f)),
        Vertex3D(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3( 0, -1,  0), glm::vec2(1.0f, 0.0f)),
        Vertex3D(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 0, -1,  0), glm::vec2(0.0f, 0.0f)),
        Vertex3D(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 0, -1,  0), glm::vec2(0.0f, 1.0f))
    };

    // Define the cube indices
    indices = {
        // Front face
        0, 1, 2,  2, 3, 0,
        // Back face
        4, 5, 6,  6, 7, 4,
        // Left face
        8, 9,10, 10,11, 8,
        // Right face
        12,13,14, 14,15,12,
        // Top face
        16,17,18, 18,19,16,
        // Bottom face
        20,21,22, 22,23,20
    };
}

// Function to set up VBO, EBO, and VAO for the cube
void setupCubeBuffers(const std::vector<Vertex3D>& vertices, const std::vector<GLuint>& indices, GLuint& cubeVBO, GLuint& cubeEBO, GLuint& cubeVAO) {
    // Generate VBO, EBO, and VAO
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);
    glGenVertexArrays(1, &cubeVAO);

    // Bind VAO
    glBindVertexArray(cubeVAO);

    // Bind VBO and upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex3D), vertices.data(), GL_STATIC_DRAW);

    // Bind EBO and upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    // Set up vertex attributes
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                      // Attribute index
        3,                      // Number of components (x, y, z)
        GL_FLOAT,               // Type
        GL_FALSE,               // Normalized?
        sizeof(Vertex3D),       // Stride
        (void*)offsetof(Vertex3D, position) // Offset
    );

    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex3D),
        (void*)offsetof(Vertex3D, normal)
    );

    // Texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex3D),
        (void*)offsetof(Vertex3D, texCoords)
    );

    // Unbind VAO (EBO remains bound to VAO)
    glBindVertexArray(0);
}

bool checkCollision(const glm::vec3& sphereCenter, float radius, const AABB& box) {
    float distanceSquared = 0.0f;

    // Calculate squared distance from sphere center to nearest AABB point
    for (int i = 0; i < 3; ++i) {
        if (sphereCenter[i] < box.min[i]) {
            distanceSquared += (box.min[i] - sphereCenter[i]) * (box.min[i] - sphereCenter[i]);
        } else if (sphereCenter[i] > box.max[i]) {
            distanceSquared += (sphereCenter[i] - box.max[i]) * (sphereCenter[i] - box.max[i]);
        }
    }
    return distanceSquared <= (radius * radius);
}

int main(int argc, char* argv[]) {
    std::cout << "Program started" << std::endl;

    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) != 0){
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
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    std::cout << "GLAD initialized" << std::endl;

    // Output OpenGL version and renderer
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    if (renderer)
        std::cout << "Renderer: " << renderer << std::endl;
    else
        std::cout << "Renderer is NULL" << std::endl;

    if (version)
        std::cout << "OpenGL version supported: " << version << std::endl;
    else
        std::cout << "OpenGL version is NULL" << std::endl;

    // Check if glGenVertexArrays is available
    if (glGenVertexArrays == nullptr) {
        std::cerr << "Error: glGenVertexArrays is NULL" << std::endl;
        return -1;
    }

    /*********************************
     * Initialization code
     *********************************/
    // Sphere creation
    glimac::Sphere sphere(1, 32, 16);
    std::cout << "Sphere created" << std::endl;

    // Create and bind VBO for sphere
    GLuint sphereVBO;
    glGenBuffers(1, &sphereVBO);
    if (sphereVBO == 0) {
        std::cerr << "Failed to generate VBO" << std::endl;
        return -1;
    }
    std::cout << "Sphere VBO generated: " << sphereVBO << std::endl;

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.getVertexCount() * sizeof(glimac::ShapeVertex), sphere.getDataPointer(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    std::cout << "Sphere VBO data buffered" << std::endl;

    // Create VAO for sphere
    GLuint sphereVAO;
    glGenVertexArrays(1, &sphereVAO);
    if (sphereVAO == 0) {
        std::cerr << "Failed to generate VAO" << std::endl;
        return -1;
    }
    std::cout << "Sphere VAO generated: " << sphereVAO << std::endl;

    // Bind VAO and set up vertex attributes for sphere
    glBindVertexArray(sphereVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                              // Attribute index (layout location in shader)
        3,                              // Number of components (x, y, z)
        GL_FLOAT,                       // Type
        GL_FALSE,                       // Normalized?
        sizeof(glimac::ShapeVertex),    // Stride
        (void*)offsetof(glimac::ShapeVertex, position) // Offset
    );

    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,                              // Attribute index
        3,                              // Number of components (x, y, z)
        GL_FLOAT,                       // Type
        GL_FALSE,                       // Normalized?
        sizeof(glimac::ShapeVertex),    // Stride
        (void*)offsetof(glimac::ShapeVertex, normal) // Offset
    );

    // Texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,                              // Attribute index
        2,                              // Number of components (u, v)
        GL_FLOAT,                       // Type
        GL_FALSE,                       // Normalized?
        sizeof(glimac::ShapeVertex),    // Stride
        (void*)offsetof(glimac::ShapeVertex, texCoords) // Offset
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    std::cout << "Sphere VAO set up" << std::endl;

    // Define AABB for the Sphere
    glm::vec3 spherePosition(0.0f, 0.0f, 0.0f); // Sphere center position
    float sphereRadius = 1.0f; // Radius of the sphere
    objectBoundaries.push_back({
        spherePosition - glm::vec3(sphereRadius), // min corner
        spherePosition + glm::vec3(sphereRadius)  // max corner
    });

    // Cube setup
    // Create cube vertices and indices
    std::vector<Vertex3D> cubeVertices;
    std::vector<GLuint> cubeIndices;
    createCube(cubeVertices, cubeIndices);

    // Set up VBO, EBO, and VAO for the cube
    GLuint cubeVBO, cubeEBO, cubeVAO;
    setupCubeBuffers(cubeVertices, cubeIndices, cubeVBO, cubeEBO, cubeVAO);
    std::cout << "Cube VBO, EBO, and VAO set up" << std::endl;

    // Cube 1 AABB
    glm::vec3 cube1Pos(2.0f, 0.0f, 0.0f); // Position of Cube 1
    float cube1Size = 0.5f; // Size (half-dimension)
    objectBoundaries.push_back({
        cube1Pos - glm::vec3(cube1Size),
        cube1Pos + glm::vec3(cube1Size)
    });

    // Cube 2 AABB
    glm::vec3 cube2Pos(-2.0f, 0.0f, 0.0f); // Position of Cube 2
    float cube2Size = 0.5f; // Size (half-dimension)
    objectBoundaries.push_back({
        cube2Pos - glm::vec3(cube2Size),
        cube2Pos + glm::vec3(cube2Size)
    });

    // Cube 3 AABB (non-uniform scaling)
    glm::vec3 cube3Pos(0.0f, 2.0f, 0.0f); // Position of Cube 3
    glm::vec3 cube3Size(0.5f, 1.0f, 0.5f); // Non-uniform dimensions
    objectBoundaries.push_back({
        cube3Pos - cube3Size,
        cube3Pos + cube3Size
    });

    for (int x = 0; x < numCubesX; ++x) {
        for (int z = 0; z < numCubesZ; ++z) {
            glm::vec3 cubePos(-7.0f + x * spacingX, -2.0f, 7.0f - z * spacingZ);
            float cubeSize = 0.5f; // Size of each floor cube
            objectBoundaries.push_back({
                cubePos - glm::vec3(cubeSize),
                cubePos + glm::vec3(cubeSize)
            });
        }
    }

    // Load shaders
    glimac::FilePath applicationPath(argv[0]);
    // Load unified shader
    std::string unifiedVertexShaderPath = applicationPath.dirPath() + "TP4/shaders/" + "unified_shader.vs.glsl";
    std::string unifiedFragmentShaderPath = applicationPath.dirPath() + "TP4/shaders/" + "unified_shader.fs.glsl";

    std::cout << "Unified Vertex shader path: " << unifiedVertexShaderPath << std::endl;
    std::cout << "Unified Fragment shader path: " << unifiedFragmentShaderPath << std::endl;

    glimac::Program unifiedProgram = glimac::loadProgram(unifiedVertexShaderPath, unifiedFragmentShaderPath);
    if (unifiedProgram.getGLId() == 0) {
        std::cerr << "Failed to load unified shaders" << std::endl;
        return -1;
    }
    std::cout << "Unified shaders loaded successfully" << std::endl;

    // Get uniform locations for unified program
    unifiedProgram.use();
    std::cout << "Unified program in use" << std::endl;

    GLint uMVPMatrixLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uMVPMatrix");
    GLint uMVMatrixLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uMVMatrix");
    GLint uNormalMatrixLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uNormalMatrix");
    GLint uTextureLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uTexture");
    GLint uObjectColorLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uObjectColor");
    GLint uUseTextureLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uUseTexture");

    // sanity check
    if (uMVPMatrixLocation == -1) std::cerr << "Failed to get 'uMVPMatrix' location" << std::endl;
    if (uMVMatrixLocation == -1) std::cerr << "Failed to get 'uMVMatrix' location" << std::endl;
    if (uNormalMatrixLocation == -1) std::cerr << "Failed to get 'uNormalMatrix' location" << std::endl;
    if (uTextureLocation == -1) std::cerr << "Failed to get 'uTexture' location" << std::endl;
    if (uObjectColorLocation == -1) std::cerr << "Failed to get 'uObjectColor' location" << std::endl;
    if (uUseTextureLocation == -1) std::cerr << "Failed to get 'uUseTexture' location" << std::endl;

    // Load the texture using glimac::Image
    std::string texturePath = applicationPath.dirPath() + "../TP4/assets/textures/cobblestone_8bit.png";
    std::cout << "Attempting to load texture at: " << texturePath << std::endl;

    std::unique_ptr<glimac::Image> pImage = glimac::loadImage(texturePath);
    if (!pImage) {
        std::cerr << "Failed to load texture image at " << texturePath << std::endl;
        return -1;
    }

    // Flip texture vertically
    size_t width = pImage->getWidth();
    size_t height = pImage->getHeight();
    glm::vec4* pixels = pImage->getPixels();

    // Iterate through each column and swap rows from top to bottom
    for (size_t row = 0; row < height / 2; ++row) {
        for (size_t col = 0; col < width; ++col) {
            size_t topIndex = row * width + col;
            size_t bottomIndex = (height - 1 - row) * width + col;

            // Swap the pixels at topIndex and bottomIndex
            std::swap(pixels[topIndex], pixels[bottomIndex]);
        }
    }

    // Generate and bind the texture
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters to nearest-neighbor, to keep the texture pixelated, sharp, and crisp
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Upload the texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pImage->getWidth(), pImage->getHeight(), 0, GL_RGBA, GL_FLOAT, pImage->getPixels());
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Set the texture unit to 0 (GL_TEXTURE0)
    unifiedProgram.use();
    glUniform1i(uTextureLocation, 0);

    // Load the new texture "stone_8bit.png"
    std::string stoneTexturePath = applicationPath.dirPath() + "../TP4/assets/textures/stone_8bit.png";
    std::cout << "Attempting to load texture at: " << stoneTexturePath << std::endl;

    std::unique_ptr<glimac::Image> pStoneImage = glimac::loadImage(stoneTexturePath);
    if (!pStoneImage) {
        std::cerr << "Failed to load texture image at " << stoneTexturePath << std::endl;
        return -1;
    }

    // Flip the stone texture vertically
    size_t stoneWidth = pStoneImage->getWidth();
    size_t stoneHeight = pStoneImage->getHeight();
    glm::vec4* stonePixels = pStoneImage->getPixels();

    // Iterate through each column and swap rows from top to bottom
    for (size_t row = 0; row < stoneHeight / 2; ++row) {
        for (size_t col = 0; col < stoneWidth; ++col) {
            size_t topIndex = row * stoneWidth + col;
            size_t bottomIndex = (stoneHeight - 1 - row) * stoneWidth + col;

            // Swap the pixels at topIndex and bottomIndex
            std::swap(stonePixels[topIndex], stonePixels[bottomIndex]);
        }
    }

    // Generate and bind the stone texture
    GLuint stoneTextureID;
    glGenTextures(1, &stoneTextureID);
    glBindTexture(GL_TEXTURE_2D, stoneTextureID);

    // Set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters to nearest-neighbor
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Upload the stone texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pStoneImage->getWidth(), pStoneImage->getHeight(), 0, GL_RGBA, GL_FLOAT, pStoneImage->getPixels());
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    std::cout << "Depth test enabled" << std::endl;

    // Set the clear color to a dark gray
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Camera parameters
    glm::vec3 cameraPos(0.0f, 0.0f, 5.0f);    // Initial position of the camera
    glm::vec3 cameraFront(0.0f, 0.0f, -1.0f); // Direction the camera is looking at
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);     // Up vector

    float cameraPosY = cameraPos.y; // Store the initial y position

    float yaw = -90.0f;  // Yaw angle initialized to -90 degrees to look towards negative Z
    float pitch = 0.0f;  // Pitch angle
    float lastX = window_width / 2.0f;  // Last mouse X position
    float lastY = window_height / 2.0f; // Last mouse Y position
    bool firstMouse = true; // First mouse movement flag

    float cameraSpeed = 5.0f; // Adjust accordingly
    float deltaTime = 0.0f;   // Time between current frame and last frame
    float lastFrame = 0.0f;   // Time of last frame

    // Enable relative mouse mode to capture mouse movement
    SDL_SetRelativeMouseMode(SDL_TRUE);

    int frameCount = 0;
    float fpsTimer = 0.0f;
    int fps = 0;

    // Main loop
    bool done = false;
    std::cout << "Entering main loop" << std::endl;
    while (!done) {
        // Calculate delta time
        float currentFrame = windowManager.getTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float adjustedSpeed = cameraSpeed * deltaTime;

        // Update FPS counter
        frameCount++;
        fpsTimer += deltaTime;
        if (fpsTimer >= 1.0f) {
            fps = frameCount;
            frameCount = 0;
            fpsTimer -= 1.0f;

            // Update window title with FPS
            std::string newTitle = "Boules - FPS: " + std::to_string(fps);
            SDL_SetWindowTitle(windowManager.getWindow(), newTitle.c_str());
        }

        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Event handling
        SDL_Event e;
        while (windowManager.pollEvent(e)) {
            if (e.type == SDL_QUIT) {
                done = true;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    done = true;
                }
            }

            // Mouse movement
            if (e.type == SDL_MOUSEMOTION) {
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
        const Uint8* state = SDL_GetKeyboardState(NULL);
        
        glm::vec3 proposedCameraPos = cameraPos; // Temporary camera position for collision testing

        // Calculate proposed camera position based on input
        if (state[SDL_SCANCODE_W]) {
            proposedCameraPos += adjustedSpeed * frontDirection;
        }
        if (state[SDL_SCANCODE_S]) {
            proposedCameraPos -= adjustedSpeed * frontDirection;
        }
        if (state[SDL_SCANCODE_A]) {
            proposedCameraPos -= rightDirection * adjustedSpeed;
        }
        if (state[SDL_SCANCODE_D]) {
            proposedCameraPos += rightDirection * adjustedSpeed;
        }

        // Check collision against all objects
        bool collisionDetected = false;
        for (const auto& boundary : objectBoundaries) {
            if (checkCollision(proposedCameraPos, cameraRadius, boundary)) {
                collisionDetected = true;
                break; // Stop further checking if a collision is found
            }
        }

        // Update camera position only if no collision is detected
        if (!collisionDetected) {
            cameraPos = proposedCameraPos;
        }

        // Keep the camera at the initial y position
        cameraPos.y = cameraPosY;

        // Define MVP matrices
        glm::mat4 ProjMatrix = glm::perspective(
            glm::radians(70.0f),                            // Field of View
            window_width / (float)window_height,            // Aspect ratio
            0.1f,                                           // Near clipping plane
            100.0f                                          // Far clipping plane
        );

        glm::mat4 ViewMatrix = glm::lookAt(
            cameraPos,             // Camera position
            cameraPos + cameraFront, // Look at target
            cameraUp               // Up vector
        );

        // Use unified program
        unifiedProgram.use();

        // **Draw the Sphere**
        // Sphere Model Matrix
        glm::mat4 sphereModelMatrix = glm::mat4(1.0f);

        glm::mat4 sphereMVMatrix = ViewMatrix * sphereModelMatrix;
        glm::mat4 sphereMVPMatrix = ProjMatrix * sphereMVMatrix;
        glm::mat4 sphereNormalMatrix = glm::transpose(glm::inverse(sphereMVMatrix));

        // Send sphere matrices to the shaders
        glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(sphereMVMatrix));
        glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(sphereMVPMatrix));
        glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(sphereNormalMatrix));

        // Set uniforms for untextured object
        glUniform1f(uUseTextureLocation, 0.0f);
        glUniform3f(uObjectColorLocation, 1.0f, 1.0f, 1.0f); // White color

        // Bind sphere VAO and draw
        glBindVertexArray(sphereVAO);
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        glBindVertexArray(0);

        // **Draw the First Cube**
        // Cube 1 Model Matrix
        glm::mat4 cube1ModelMatrix = glm::mat4(1.0f);
        cube1ModelMatrix = glm::translate(cube1ModelMatrix, glm::vec3(2.0f, 0.0f, 0.0f)); // Position cube next to sphere
        cube1ModelMatrix = glm::scale(cube1ModelMatrix, glm::vec3(0.5f)); // Scale down the cube uniformly

        glm::mat4 cube1MVMatrix = ViewMatrix * cube1ModelMatrix;
        glm::mat4 cube1MVPMatrix = ProjMatrix * cube1MVMatrix;
        glm::mat4 cube1NormalMatrix = glm::transpose(glm::inverse(cube1MVMatrix));

        // Send cube matrices to the shaders
        glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(cube1MVMatrix));
        glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(cube1MVPMatrix));
        glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(cube1NormalMatrix));

        // Set uniforms for untextured object
        glUniform1f(uUseTextureLocation, 0.0f);
        glUniform3f(uObjectColorLocation, 0.6f, 0.6f, 0.6f); // Gray color

        // Bind cube VAO and draw
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cubeIndices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // **Draw the Second Cube**
        // Cube 2 Model Matrix
        glm::mat4 cube2ModelMatrix = glm::mat4(1.0f);
        cube2ModelMatrix = glm::translate(cube2ModelMatrix, glm::vec3(-2.0f, 0.0f, 0.0f)); // Position cube to the left
        cube2ModelMatrix = glm::scale(cube2ModelMatrix, glm::vec3(0.5f)); // Same uniform scale

        glm::mat4 cube2MVMatrix = ViewMatrix * cube2ModelMatrix;
        glm::mat4 cube2MVPMatrix = ProjMatrix * cube2MVMatrix;
        glm::mat4 cube2NormalMatrix = glm::transpose(glm::inverse(cube2MVMatrix));

        // Send cube matrices to the shaders
        glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(cube2MVMatrix));
        glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(cube2MVPMatrix));
        glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(cube2NormalMatrix));

        // Set uniforms for untextured object
        glUniform1f(uUseTextureLocation, 0.0f);
        glUniform3f(uObjectColorLocation, 0.6f, 0.6f, 0.6f); // Gray color

        // Bind cube VAO and draw
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cubeIndices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // **Draw the Third Cube**
        // Cube 3 Model Matrix
        glm::mat4 cube3ModelMatrix = glm::mat4(1.0f);
        cube3ModelMatrix = glm::translate(cube3ModelMatrix, glm::vec3(0.0f, 2.0f, 0.0f)); // Position cube above the sphere
        cube3ModelMatrix = glm::scale(cube3ModelMatrix, glm::vec3(0.5f, 1.0f, 0.5f)); // Non-uniform scaling

        glm::mat4 cube3MVMatrix = ViewMatrix * cube3ModelMatrix;
        glm::mat4 cube3MVPMatrix = ProjMatrix * cube3MVMatrix;
        glm::mat4 cube3NormalMatrix = glm::transpose(glm::inverse(cube3MVMatrix));

        // Send cube matrices to the shaders
        glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(cube3MVMatrix));
        glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(cube3MVPMatrix));
        glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(cube3NormalMatrix));

        // Set uniforms for untextured object
        glUniform1f(uUseTextureLocation, 0.0f);
        glUniform3f(uObjectColorLocation, 0.6f, 0.6f, 0.6f); // Gray color

        // Bind cube VAO and draw
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cubeIndices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // **Draw a Textured Cube**
        // Textured Cube Model Matrix
        glm::mat4 texturedCubeModelMatrix = glm::mat4(1.0f);
        texturedCubeModelMatrix = glm::translate(texturedCubeModelMatrix, glm::vec3(0.0f, 0.0f, 2.0f)); // Position cube
        texturedCubeModelMatrix = glm::scale(texturedCubeModelMatrix, glm::vec3(0.5f)); // Scale down the cube

        glm::mat4 texturedCubeMVMatrix = ViewMatrix * texturedCubeModelMatrix;
        glm::mat4 texturedCubeMVPMatrix = ProjMatrix * texturedCubeMVMatrix;
        glm::mat4 texturedCubeNormalMatrix = glm::transpose(glm::inverse(texturedCubeMVMatrix));

        // Send matrices to the shaders
        glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(texturedCubeMVMatrix));
        glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(texturedCubeMVPMatrix));
        glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(texturedCubeNormalMatrix));

        // Set uniforms for textured object
        glUniform1f(uUseTextureLocation, 1.0f);

        // Bind texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(uTextureLocation, 0);

        // Bind cube VAO and draw
        glBindVertexArray(cubeVAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cubeIndices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        for (int x = 0; x < numCubesX; ++x) {
            for (int z = 0; z < numCubesZ; ++z) {
                // **Draw Each Textured Cube in a Grid**

                // New Textured Cube Model Matrix for each cube with incremental x and z positions
                glm::mat4 newTexturedCubeModelMatrix = glm::mat4(1.0f);
                newTexturedCubeModelMatrix = glm::translate(
                    newTexturedCubeModelMatrix,
                    glm::vec3(-7.0f + x * spacingX, -2.0f, 7.0f - z * spacingZ) // Position in x and z
                );
                newTexturedCubeModelMatrix = glm::scale(newTexturedCubeModelMatrix, glm::vec3(1.0f)); // Scale each cube

                glm::mat4 newTexturedCubeMVMatrix = ViewMatrix * newTexturedCubeModelMatrix;
                glm::mat4 newTexturedCubeMVPMatrix = ProjMatrix * newTexturedCubeMVMatrix;
                glm::mat4 newTexturedCubeNormalMatrix = glm::transpose(glm::inverse(newTexturedCubeMVMatrix));

                // Send matrices to the shaders
                glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(newTexturedCubeMVMatrix));
                glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(newTexturedCubeMVPMatrix));
                glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(newTexturedCubeNormalMatrix));

                // Set uniforms for textured object
                glUniform1f(uUseTextureLocation, 1.0f);

                // Bind stone texture
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, stoneTextureID);
                glUniform1i(uTextureLocation, 0);

                // Bind cube VAO and draw
                glBindVertexArray(cubeVAO);
                glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(cubeIndices.size()), GL_UNSIGNED_INT, 0);
            }
        }

        // Unbind the VAO after the loops
        glBindVertexArray(0);

        // Swap buffers
        windowManager.swapBuffers();
    }

    // Clean up sphere buffers
    glDeleteBuffers(1, &sphereVBO);
    glDeleteVertexArrays(1, &sphereVAO);

    // Clean up cube buffers
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);
    glDeleteVertexArrays(1, &cubeVAO);

    // Clean up texture
    glDeleteTextures(1, &textureID);

    std::cout << "Program terminated successfully" << std::endl;

    return 0;
}