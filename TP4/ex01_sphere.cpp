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
#include <map>

int window_width = 800;
int window_height = 800;
int shadowMapSize = 1024;
float farPlane = 25.0f; // Far plane for shadow calculations

// glimac::Program unifiedProgram; // Global shader program object

// Uniform locations
GLint uMVPMatrixLocation;
GLint uMVMatrixLocation;
GLint uNormalMatrixLocation;
GLint uTextureLocation;
GLint uObjectColorLocation;
GLint uUseTextureLocation;

// Stone floor coordinates
int numCubesX = 15; // Number of cubes along the x-axis
int numCubesZ = 15; // Number of cubes along the z-axis
float spacingX = 1.0f; // Distance between each cube along the x-axis
float spacingZ = 1.0f; // Distance between each cube along the z-axis

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};

float cameraRadius = 0.15f; // Radius of the camera sphere for collision detection

// Structure for 3D vertices with position, normal, and texture coordinates
struct Vertex3D {
    glm::vec3 position;    // Vertex position
    glm::vec3 normal;      // Vertex normal
    glm::vec2 texCoords;   // Texture coordinates

    Vertex3D(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& uv)
        : position(pos), normal(norm), texCoords(uv) {}
};

enum class ObjectType {
    Cube,
    Sphere
};

struct SceneObject {
    ObjectType type;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 color;
    bool useTexture;
    GLuint textureID;       // Texture ID; use 0 if no texture
    glm::vec3 rotationAxis; // Optional, if you want to support rotation
    float rotationAngle;    // Optional, in degrees
    AABB boundingBox;       // Axis-Aligned Bounding Box for collision
    GLuint vaoID;           // VAO for the object
    GLsizei indexCount;     // Number of indices to draw (if using EBO)
};

std::vector<SceneObject> sceneObjects; // List of all scene objects
std::map<std::string, GLuint> textures; // Map to store loaded textures

// Light properties
glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, 5.0f); // Light source position
float lightIntensity = 20.0f;
float minBias = 0.05f;
float maxBias = 0.5f;

// Directions and up vectors for each cubemap face
const glm::vec3 directions[6] = {
    glm::vec3(1.0f, 0.0f, 0.0f),  // +X
    glm::vec3(-1.0f, 0.0f, 0.0f), // -X
    glm::vec3(0.0f, 1.0f, 0.0f),  // +Y
    glm::vec3(0.0f, -1.0f, 0.0f), // -Y
    glm::vec3(0.0f, 0.0f, 1.0f),  // +Z
    glm::vec3(0.0f, 0.0f, -1.0f)  // -Z
};

const glm::vec3 upVectors[6] = {
    glm::vec3(0.0f, -1.0f, 0.0f), // For +X
    glm::vec3(0.0f, -1.0f, 0.0f), // For -X
    glm::vec3(0.0f, 0.0f, -1.0f), // For +Y
    glm::vec3(0.0f, 0.0f, 1.0f),  // For -Y
    glm::vec3(0.0f, -1.0f, 0.0f), // For +Z
    glm::vec3(0.0f, -1.0f, 0.0f)  // For -Z
};

// Shadow map cubemap for point light
GLuint shadowMap;
GLuint shadowMapFBO;



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
        Vertex3D(glm::vec3( 0.5f,  0.5f, -0.5f), glm::vec3( 0,  0, -1), glm::vec2(1.0f, 1.0f)),
        Vertex3D(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3( 0,  0, -1), glm::vec2(1.0f, 1.0f)),
        // Left face
        Vertex3D(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1,  0,  0), glm::vec2(0.0f, 0.0f)),
        Vertex3D(glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec3(-1,  0,  0), glm::vec2(1.0f, 0.0f)),
        Vertex3D(glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec3(-1,  0,  0), glm::vec2(1.0f, 1.0f)),
        Vertex3D(glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec3(-1,  0,  0), glm::vec2(0.0f, 1.0f)),
        // Right face
        Vertex3D(glm::vec3( 0.5f, -0.5f, -0.5f), glm::vec3( 1,  0,  0), glm::vec2(1.0f, 0.0f)),
        Vertex3D(glm::vec3( 0.5f, -0.5f,  0.5f), glm::vec3( 1,  0,  0), glm::vec2(0.0f, 0.0f)),
        Vertex3D(glm::vec3( 0.5f,  0.5f,  0.5f), glm::vec3( 1,  0,  0), glm::vec2(1.0f, 1.0f)),
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

GLuint loadTexture(const std::string& texturePath) {
    if (textures.find(texturePath) != textures.end()) {
        return textures[texturePath];
    }

    // Load the texture using glimac::Image
    std::unique_ptr<glimac::Image> pImage = glimac::loadImage(texturePath);
    if (!pImage) {
        std::cerr << "Failed to load texture image at " << texturePath << std::endl;
        return 0;
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

    // Store in map
    textures[texturePath] = textureID;
    return textureID;
}

void addCube(
    const glm::vec3& position,
    const glm::vec3& scale,
    const glm::vec3& color,
    bool useTexture,
    GLuint textureID = 0, // Default to 0 if no texture
    const glm::vec3& rotationAxis = glm::vec3(0.0f),
    float rotationAngle = 0.0f,
    GLuint vaoID = 0,
    GLsizei indexCount = 0
) {
    SceneObject cube;
    cube.type = ObjectType::Cube;
    cube.position = position;
    cube.scale = scale;
    cube.color = color;
    cube.useTexture = useTexture;
    cube.textureID = textureID;
    cube.rotationAxis = rotationAxis;
    cube.rotationAngle = rotationAngle;
    cube.vaoID = vaoID;
    cube.indexCount = indexCount;

    // Calculate bounding box
    glm::vec3 halfSize = scale * 0.5f;
    cube.boundingBox.min = position - halfSize;
    cube.boundingBox.max = position + halfSize;

    // Add to scene objects
    sceneObjects.push_back(cube);
}

void addSphere(
    const glm::vec3& position,
    float radius,
    const glm::vec3& color,
    bool useTexture,
    GLuint textureID = 0, // Default to 0 if no texture
    GLuint vaoID = 0,
    GLsizei vertexCount = 0
) {
    SceneObject sphereObject;
    sphereObject.type = ObjectType::Sphere;
    sphereObject.position = position;
    sphereObject.scale = glm::vec3(radius * 1.0f);
    sphereObject.color = color;
    sphereObject.useTexture = useTexture;
    sphereObject.textureID = textureID;
    sphereObject.vaoID = vaoID;
    sphereObject.indexCount = vertexCount;

    // Calculate bounding box
    sphereObject.boundingBox.min = position - glm::vec3(radius);
    sphereObject.boundingBox.max = position + glm::vec3(radius);

    // Add to scene objects
    sceneObjects.push_back(sphereObject);
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

void renderSceneFromLight(const glimac::Program& unifiedProgram) {
    for (const auto& object : sceneObjects) {
        // Create Model Matrix for each object
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, object.position);
        if (object.rotationAngle != 0.0f) {
            modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotationAngle), object.rotationAxis);
        }
        modelMatrix = glm::scale(modelMatrix, object.scale);

        // Pass model matrix to the shader (depth shader should include this uniform)
        glUniformMatrix4fv(glGetUniformLocation(unifiedProgram.getGLId(), "uModelMatrix"), 1, GL_FALSE, glm::value_ptr(modelMatrix));

        // Bind VAO and render for each object type
        glBindVertexArray(object.vaoID);
        if (object.type == ObjectType::Cube) {
            glDrawElements(GL_TRIANGLES, object.indexCount, GL_UNSIGNED_INT, 0);
        } else if (object.type == ObjectType::Sphere) {
            glDrawArrays(GL_TRIANGLES, 0, object.indexCount);
        }
        glBindVertexArray(0);
    }
}

glm::mat4 ProjMatrix;
glm::mat4 ViewMatrix;

void renderScene(const glimac::Program& unifiedProgram) {
    unifiedProgram.use();
    for (const auto& object : sceneObjects) {
        // Create Model Matrix for each object
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, object.position);
        if (object.rotationAngle != 0.0f) {
            modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotationAngle), object.rotationAxis);
        }
        modelMatrix = glm::scale(modelMatrix, object.scale);

        // Set MVP matrices for the shaders
        glm::mat4 mvMatrix = ViewMatrix * modelMatrix;
        glm::mat4 mvpMatrix = ProjMatrix * mvMatrix;
        glm::mat4 normalMatrix = glm::transpose(glm::inverse(mvMatrix));

        glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvMatrix));
        glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(mvpMatrix));
        glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        // Set object-specific uniforms
        glUniform1f(uUseTextureLocation, object.useTexture ? 1.0f : 0.0f);
        glUniform3fv(uObjectColorLocation, 1, glm::value_ptr(object.color));

        if (object.useTexture && object.textureID != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, object.textureID);
        }

        // Bind the correct VAO
        glBindVertexArray(object.vaoID);

        // Draw based on object type
        if (object.type == ObjectType::Cube) {
            glDrawElements(GL_TRIANGLES, object.indexCount, GL_UNSIGNED_INT, 0);
        } else if (object.type == ObjectType::Sphere) {
            glDrawArrays(GL_TRIANGLES, 0, object.indexCount);
        }

        glBindVertexArray(0);

        if (object.useTexture && object.textureID != 0) {
            glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture
        }
    }
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

    // Cube setup
    // Create cube vertices and indices
    std::vector<Vertex3D> cubeVertices;
    std::vector<GLuint> cubeIndices;
    createCube(cubeVertices, cubeIndices);

    // Set up VBO, EBO, and VAO for the cube
    GLuint cubeVBO, cubeEBO, cubeVAO;
    setupCubeBuffers(cubeVertices, cubeIndices, cubeVBO, cubeEBO, cubeVAO);
    std::cout << "Cube VBO, EBO, and VAO set up" << std::endl;

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

    uMVPMatrixLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uMVPMatrix");
    uMVMatrixLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uMVMatrix");
    uNormalMatrixLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uNormalMatrix");
    uTextureLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uTexture");
    uObjectColorLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uObjectColor");
    uUseTextureLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uUseTexture");
    GLint lightMatrixLoc = glGetUniformLocation(unifiedProgram.getGLId(), "uLightSpaceMatrix");
    if (lightMatrixLoc == -1) {
        std::cerr << "Failed to get 'uLightSpaceMatrix' location" << std::endl;
    }

    // Sanity check
    if (uMVPMatrixLocation == -1) std::cerr << "Failed to get 'uMVPMatrix' location" << std::endl;
    if (uMVMatrixLocation == -1) std::cerr << "Failed to get 'uMVMatrix' location" << std::endl;
    if (uNormalMatrixLocation == -1) std::cerr << "Failed to get 'uNormalMatrix' location" << std::endl;
    if (uTextureLocation == -1) std::cerr << "Failed to get 'uTexture' location" << std::endl;
    if (uObjectColorLocation == -1) std::cerr << "Failed to get 'uObjectColor' location" << std::endl;
    if (uUseTextureLocation == -1) std::cerr << "Failed to get 'uUseTexture' location" << std::endl;

    // Load textures
    GLuint textureID = loadTexture(applicationPath.dirPath() + "../TP4/assets/textures/cobblestone_8bit.png");
    GLuint stoneTextureID = loadTexture(applicationPath.dirPath() + "../TP4/assets/textures/stone_8bit.png");

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    std::cout << "Depth test enabled" << std::endl;

    // Initialize shadow map framebuffer and cubemap texture
    glGenFramebuffers(1, &shadowMapFBO);
    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, shadowMapSize, shadowMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Shadow Map FBO not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Set the clear color to a dark gray
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Camera parameters
    glm::vec3 cameraPos(0.0f, 0.0f, 5.0f);    // Initial position of the camera
    glm::vec3 cameraFront(0.0f, 0.0f, -1.0f); // Direction the camera is looking at
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);     // Up vector

    float cameraPosY = cameraPos.y; // Store the initial y position

    float yaw = -90.0f;  // Yaw angle initialized to -90 degrees to look towards negative Z
    float pitch = 0.0f;  // Pitch angle

    float cameraSpeed = 5.0f; // Adjust accordingly
    float deltaTime = 0.0f;   // Time between current frame and last frame
    float lastFrame = 0.0f;   // Time of last frame

    // Enable relative mouse mode to capture mouse movement
    SDL_SetRelativeMouseMode(SDL_TRUE);

    int frameCount = 0;
    float fpsTimer = 0.0f;
    int fps = 0;

    // Prepare to add objects to the scene
    GLsizei cubeIndexCount = static_cast<GLsizei>(cubeIndices.size());
    GLsizei sphereVertexCount = sphere.getVertexCount();

    // Add sphere to the scene
    addSphere(
        glm::vec3(0.0f, 0.0f, 0.0f), // Position
        1.0f,                        // Radius
        glm::vec3(1.0f),             // Color (white)
        false,                       // Use texture
        0,                           // Texture ID
        sphereVAO,                   // VAO ID
        sphereVertexCount            // Vertex count
    );

    // Add cubes to the scene
    addCube(
        glm::vec3(2.0f, 0.0f, 0.0f), // Position
        glm::vec3(0.5f),             // Scale
        glm::vec3(0.6f),             // Color (gray)
        false,                       // Use texture
        0,                           // Texture ID
        glm::vec3(0.0f),             // Rotation axis
        0.0f,                        // Rotation angle
        cubeVAO,                     // VAO ID
        cubeIndexCount               // Index count
    );

    addCube(
        glm::vec3(-2.0f, 0.0f, 0.0f), // Position
        glm::vec3(0.5f),              // Scale
        glm::vec3(0.6f),              // Color (gray)
        false,                        // Use texture
        0,                            // Texture ID
        glm::vec3(0.0f),              // Rotation axis
        0.0f,                         // Rotation angle
        cubeVAO,                      // VAO ID
        cubeIndexCount                // Index count
    );

    addCube(
        glm::vec3(0.0f, 2.0f, 0.0f),  // Position
        glm::vec3(0.5f, 1.0f, 0.5f),  // Scale (non-uniform)
        glm::vec3(0.6f),              // Color (gray)
        false,                        // Use texture
        0,                            // Texture ID
        glm::vec3(0.0f),              // Rotation axis
        0.0f,                         // Rotation angle
        cubeVAO,                      // VAO ID
        cubeIndexCount                // Index count
    );

    // Add textured cube
    addCube(
        glm::vec3(0.0f, 0.0f, 2.0f),  // Position
        glm::vec3(0.5f),              // Scale
        glm::vec3(1.0f),              // Color (white, not used)
        true,                         // Use texture
        textureID,                    // Texture ID
        glm::vec3(0.0f),              // Rotation axis
        0.0f,                         // Rotation angle
        cubeVAO,                      // VAO ID
        cubeIndexCount                // Index count
    );

    // Add floor grid cubes
    for (int x = 0; x < numCubesX; ++x) {
        for (int z = 0; z < numCubesZ; ++z) {
            glm::vec3 position(-7.0f + x * spacingX, -2.0f, 7.0f - z * spacingZ);
            addCube(
                position,                   // Position
                glm::vec3(1.0f),            // Scale
                glm::vec3(1.0f),            // Color (white, not used)
                true,                       // Use texture
                stoneTextureID,             // Texture ID
                glm::vec3(0.0f),            // Rotation axis
                0.0f,                       // Rotation angle
                cubeVAO,                    // VAO ID
                cubeIndexCount              // Index count
            );
        }
    }

    // Main loop variables
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
        for (const auto& object : sceneObjects) {
            if (checkCollision(proposedCameraPos, cameraRadius, object.boundingBox)) {
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

        // Redefine MVP matrices
        ProjMatrix = glm::perspective(
            glm::radians(70.0f),
            window_width / (float)window_height,
            0.1f,
            100.0f
        );

        ViewMatrix = glm::lookAt(
            cameraPos,
            cameraPos + cameraFront,
            cameraUp
        );

        // Render shadow map from light's perspective
        glViewport(0, 0, shadowMapSize, shadowMapSize);
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        for (int i = 0; i < 6; ++i) {
            glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, farPlane);
            glm::mat4 lightView = glm::lookAt(lightPos, lightPos + directions[i], upVectors[i]);
            glm::mat4 lightSpaceMatrix = lightProjection * lightView;

            // Pass the light space matrix to shader for current cubemap face
            glUniformMatrix4fv(lightMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

            // Clear depth buffer for each face and render scene from light's perspective
            glClear(GL_DEPTH_BUFFER_BIT);
            renderSceneFromLight(unifiedProgram);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Set viewport back to normal screen size and clear buffers
        glViewport(0, 0, window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Bind the shadow cubemap for sampling in the fragment shader
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, shadowMap);
        glUniform1i(glGetUniformLocation(unifiedProgram.getGLId(), "uShadowMap"), 1);

        // Proceed with regular scene rendering
        renderScene(unifiedProgram);

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

    // Clean up textures
    for (const auto& tex : textures) {
        glDeleteTextures(1, &tex.second);
    }

    glDeleteFramebuffers(1, &shadowMapFBO);
    glDeleteTextures(1, &shadowMap);

    std::cout << "Program terminated successfully" << std::endl;

    return 0;
}