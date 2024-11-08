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

int window_width = 1200;
int window_height = 800;

// Stone floor coordinates
int numCubesX = 15;    // Number of cubes along the x-axis
int numCubesZ = 15;    // Number of cubes along the z-axis
float spacingX = 1.0f; // Distance between each cube along the x-axis
float spacingZ = 1.0f; // Distance between each cube along the z-axis

struct AABB
{
    glm::vec3 min;
    glm::vec3 max;
};

float cameraRadius = 0.15f; // Radius of the camera sphere for collision detection
float cameraHeight = 2.0f;  // Height of the camera cylinder

// Structure for 3D vertices with position, normal, and texture coordinates
struct Vertex3D
{
    glm::vec3 position;  // Vertex position
    glm::vec3 normal;    // Vertex normal
    glm::vec2 texCoords; // Texture coordinates
    glm::vec3 tangent;
    glm::vec3 bitangent;

    Vertex3D(const glm::vec3 &pos, const glm::vec3 &norm, const glm::vec2 &uv)
        : position(pos), normal(norm), texCoords(uv) {}
};

struct SphereVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

enum class ObjectType
{
    Cube,
    Sphere
};

struct SceneObject
{
    ObjectType type;
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 color;
    bool useTexture;
    GLuint textureID;       // Texture ID; use 0 if no texture
    GLuint normalMapID;     // Normal map ID; use 0 if no normal map
    glm::vec3 rotationAxis; // Optional, if need support rotation
    float rotationAngle;    // Optional, in degrees
    AABB boundingBox;       // Axis-Aligned Bounding Box for collision
    GLuint vaoID;           // VAO for the object
    GLsizei indexCount;     // Number of indices to draw (if using EBO)
};

std::vector<SceneObject> sceneObjects;  // List of all scene objects
std::map<std::string, GLuint> textures; // Map to store loaded textures

// Function to generate cube vertices and indices
void createCube(std::vector<Vertex3D> &vertices, std::vector<GLuint> &indices)
{
    // Define the cube vertices
    vertices = {
        // Front face
        Vertex3D(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(0.0f, 0.0f)),
        Vertex3D(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(1.0f, 0.0f)),
        Vertex3D(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(1.0f, 1.0f)),
        Vertex3D(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0, 0, 1), glm::vec2(0.0f, 1.0f)),
        // Back face
        Vertex3D(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(1.0f, 0.0f)),
        Vertex3D(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(0.0f, 0.0f)),
        Vertex3D(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(0.0f, 1.0f)),
        Vertex3D(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 0, -1), glm::vec2(1.0f, 1.0f)),
        // Left face
        Vertex3D(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(-1, 0, 0), glm::vec2(0.0f, 0.0f)),
        Vertex3D(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(-1, 0, 0), glm::vec2(1.0f, 0.0f)),
        Vertex3D(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(-1, 0, 0), glm::vec2(1.0f, 1.0f)),
        Vertex3D(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(-1, 0, 0), glm::vec2(0.0f, 1.0f)),
        // Right face
        Vertex3D(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(1.0f, 0.0f)),
        Vertex3D(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(0.0f, 0.0f)),
        Vertex3D(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1, 0, 0), glm::vec2(0.0f, 1.0f)),
        Vertex3D(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1, 0, 0), glm::vec2(1.0f, 1.0f)),
        // Top face
        Vertex3D(glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0, 1, 0), glm::vec2(0.0f, 1.0f)),
        Vertex3D(glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0, 1, 0), glm::vec2(0.0f, 0.0f)),
        Vertex3D(glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0, 1, 0), glm::vec2(1.0f, 0.0f)),
        Vertex3D(glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(0, 1, 0), glm::vec2(1.0f, 1.0f)),
        // Bottom face
        Vertex3D(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0, -1, 0), glm::vec2(1.0f, 1.0f)),
        Vertex3D(glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0, -1, 0), glm::vec2(1.0f, 0.0f)),
        Vertex3D(glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0, -1, 0), glm::vec2(0.0f, 0.0f)),
        Vertex3D(glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(0, -1, 0), glm::vec2(0.0f, 1.0f))};

    // Define the cube indices
    indices = {
        // Front face
        0, 1, 2, 2, 3, 0,
        // Back face
        4, 5, 6, 6, 7, 4,
        // Left face
        8, 9, 10, 10, 11, 8,
        // Right face
        12, 13, 14, 14, 15, 12,
        // Top face
        16, 17, 18, 18, 19, 16,
        // Bottom face
        20, 21, 22, 22, 23, 20};
}

void computeTangents(std::vector<Vertex3D>& vertices, const std::vector<GLuint>& indices) {
    // Initialize tangents and bitangents to zero
    for (auto& vertex : vertices) {
        vertex.tangent = glm::vec3(0.0f);
        vertex.bitangent = glm::vec3(0.0f);
    }

    // Compute per-triangle tangents and bitangents
    for (size_t i = 0; i < indices.size(); i += 3) {
        GLuint i0 = indices[i];
        GLuint i1 = indices[i + 1];
        GLuint i2 = indices[i + 2];

        Vertex3D& v0 = vertices[i0];
        Vertex3D& v1 = vertices[i1];
        Vertex3D& v2 = vertices[i2];

        glm::vec3 edge1 = v1.position - v0.position;
        glm::vec3 edge2 = v2.position - v0.position;

        glm::vec2 deltaUV1 = v1.texCoords - v0.texCoords;
        glm::vec2 deltaUV2 = v2.texCoords - v0.texCoords;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent, bitangent;

        tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
        bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

        v0.tangent += tangent;
        v1.tangent += tangent;
        v2.tangent += tangent;

        v0.bitangent += bitangent;
        v1.bitangent += bitangent;
        v2.bitangent += bitangent;
    }

    // Normalize tangents and bitangents
    for (auto& vertex : vertices) {
        vertex.tangent = glm::normalize(vertex.tangent);
        vertex.bitangent = glm::normalize(vertex.bitangent);
    }
}

void computeSphereTangents(
    std::vector<SphereVertex>& vertices,
    const std::vector<uint32_t>& indices)
{
    // Initialize tangents and bitangents to zero
    for (auto& vertex : vertices) {
        vertex.tangent = glm::vec3(0.0f);
        vertex.bitangent = glm::vec3(0.0f);
    }

    // Compute per-triangle tangents and bitangents
    for (size_t i = 0; i < indices.size(); i += 3) {
        uint32_t i0 = indices[i];
        uint32_t i1 = indices[i + 1];
        uint32_t i2 = indices[i + 2];

        SphereVertex& v0 = vertices[i0];
        SphereVertex& v1 = vertices[i1];
        SphereVertex& v2 = vertices[i2];

        glm::vec3 edge1 = v1.position - v0.position;
        glm::vec3 edge2 = v2.position - v0.position;

        glm::vec2 deltaUV1 = v1.texCoords - v0.texCoords;
        glm::vec2 deltaUV2 = v2.texCoords - v0.texCoords;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
        glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

        v0.tangent += tangent;
        v1.tangent += tangent;
        v2.tangent += tangent;

        v0.bitangent += bitangent;
        v1.bitangent += bitangent;
        v2.bitangent += bitangent;
    }

    // Normalize the tangents and bitangents
    for (auto& vertex : vertices) {
        vertex.tangent = glm::normalize(vertex.tangent);
        vertex.bitangent = glm::normalize(vertex.bitangent);
    }
}

// Function to set up VBO, EBO, and VAO for the cube
void setupCubeBuffers(const std::vector<Vertex3D> &vertices, const std::vector<GLuint> &indices, GLuint &cubeVBO, GLuint &cubeEBO, GLuint &cubeVAO)
{
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
        0,                                   // Attribute index
        3,                                   // Number of components (x, y, z)
        GL_FLOAT,                            // Type
        GL_FALSE,                            // Normalized?
        sizeof(Vertex3D),                    // Stride
        (void *)offsetof(Vertex3D, position) // Offset
    );

    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex3D),
        (void *)offsetof(Vertex3D, normal));

    // Texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex3D),
        (void *)offsetof(Vertex3D, texCoords));

    // Tangent attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3, 
        3, 
        GL_FLOAT, GL_FALSE, 
        sizeof(Vertex3D), 
        (void*)offsetof(Vertex3D, tangent));

    // Bitangent attribute
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(
        4, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(Vertex3D), 
        (void*)offsetof(Vertex3D, bitangent));

    // Unbind VAO (EBO remains bound to VAO)
    glBindVertexArray(0);
}

GLuint loadTexture(const std::string &texturePath)
{
    if (textures.find(texturePath) != textures.end())
    {
        return textures[texturePath];
    }

    // Load the texture using glimac::Image
    std::unique_ptr<glimac::Image> pImage = glimac::loadImage(texturePath);
    if (!pImage)
    {
        std::cerr << "Failed to load texture image at " << texturePath << std::endl;
        return 0;
    }

    // Flip texture vertically
    size_t width = pImage->getWidth();
    size_t height = pImage->getHeight();
    glm::vec4 *pixels = pImage->getPixels();

    // Iterate through each column and swap rows from top to bottom
    for (size_t row = 0; row < height / 2; ++row)
    {
        for (size_t col = 0; col < width; ++col)
        {
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
    const glm::vec3 &position,
    const glm::vec3 &scale,
    const glm::vec3 &color,
    bool useTexture,
    GLuint textureID = 0, // Default to 0 if no texture
    GLuint normalMapID = 0, // Default to 0 if no normal map
    const glm::vec3 &rotationAxis = glm::vec3(0.0f),
    float rotationAngle = 0.0f,
    GLuint vaoID = 0,
    GLsizei indexCount = 0)
{
    SceneObject cube;
    cube.type = ObjectType::Cube;
    cube.position = position;
    cube.scale = scale;
    cube.color = color;
    cube.useTexture = useTexture;
    cube.textureID = textureID;
    cube.normalMapID = normalMapID;
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
    const glm::vec3 &position,
    float radius,
    const glm::vec3 &color,
    bool useTexture,
    GLuint textureID = 0, // Default to 0 if no texture
    GLuint vaoID = 0,
    GLsizei vertexCount = 0)
{
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

bool checkCollision(const glm::vec3& cylinderBaseCenter, float radius, float height, const AABB& box) {
    float horizontalDistanceSquared = 0.0f;

    // Check horizontal distance (x and z axes)
    for (int i = 0; i < 3; i += 2) { // i = 0 for x, i = 2 for z
        if (cylinderBaseCenter[i] < box.min[i]) {
            horizontalDistanceSquared += (box.min[i] - cylinderBaseCenter[i]) * (box.min[i] - cylinderBaseCenter[i]);
        } else if (cylinderBaseCenter[i] > box.max[i]) {
            horizontalDistanceSquared += (cylinderBaseCenter[i] - box.max[i]) * (cylinderBaseCenter[i] - box.max[i]);
        }
    }

    // Check if horizontal distance exceeds radius
    if (horizontalDistanceSquared > (radius * radius)) {
        return false;
    }

    // Check vertical (y-axis) overlap
    float cylinderTop = cylinderBaseCenter.y + height / 2.0f;
    float cylinderBottom = cylinderBaseCenter.y - height / 2.0f;
    if (cylinderTop < box.min.y || cylinderBottom > box.max.y) {
        return false;
    }

    return true; // Collision detected
}

// Function to generate a random float between 0 and 1
float randomFloat()
{
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

// Function to generate a random color
glm::vec3 randomColor()
{
    // return glm::vec3(randomFloat(), randomFloat(), randomFloat());
    return glm::vec3(0.6f);
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

    /*********************************
     * Initialization code
     *********************************/
    // Sphere creation
    glimac::Sphere sphere(1, 32, 16);
    std::cout << "Sphere created" << std::endl;

    // Create and bind VBO for sphere
    GLuint sphereVBO;
    glGenBuffers(1, &sphereVBO);
    if (sphereVBO == 0)
    {
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
    if (sphereVAO == 0)
    {
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
        0,                                              // Attribute index (layout location in shader)
        3,                                              // Number of components (x, y, z)
        GL_FLOAT,                                       // Type
        GL_FALSE,                                       // Normalized?
        sizeof(glimac::ShapeVertex),                    // Stride
        (void *)offsetof(glimac::ShapeVertex, position) // Offset
    );

    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,                                            // Attribute index
        3,                                            // Number of components (x, y, z)
        GL_FLOAT,                                     // Type
        GL_FALSE,                                     // Normalized?
        sizeof(glimac::ShapeVertex),                  // Stride
        (void *)offsetof(glimac::ShapeVertex, normal) // Offset
    );

    // Texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,                                               // Attribute index
        2,                                               // Number of components (u, v)
        GL_FLOAT,                                        // Type
        GL_FALSE,                                        // Normalized?
        sizeof(glimac::ShapeVertex),                     // Stride
        (void *)offsetof(glimac::ShapeVertex, texCoords) // Offset
    );

    // Tangent attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(
        3, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(SphereVertex), (void*)offsetof(SphereVertex, tangent)
    );

    // Bitangent attribute
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(
        4, 
        3, 
        GL_FLOAT, 
        GL_FALSE, 
        sizeof(SphereVertex), (void*)offsetof(SphereVertex, bitangent)
    );

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    std::cout << "Sphere VAO set up" << std::endl;

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

    // Load unified shader
    std::string unifiedVertexShaderPath = applicationPath.dirPath() + "TP4/shaders/" + "unified_shader.vs.glsl";
    std::string unifiedFragmentShaderPath = applicationPath.dirPath() + "TP4/shaders/" + "directionallight.fs.glsl";

    std::cout << "Unified Vertex shader path: " << unifiedVertexShaderPath << std::endl;
    std::cout << "Unified Fragment shader path: " << unifiedFragmentShaderPath << std::endl;

    glimac::Program unifiedProgram = glimac::loadProgram(unifiedVertexShaderPath, unifiedFragmentShaderPath);
    if (unifiedProgram.getGLId() == 0)
    {
        std::cerr << "Failed to load unified shaders" << std::endl;
        return -1;
    }
    std::cout << "Unified shaders loaded successfully" << std::endl;

    // Load depth shader program for shadow mapping
    std::string depthVertexShaderPath = applicationPath.dirPath() + "TP4/shaders/" + "shadow_mapping_depth.vs.glsl";
    std::string depthFragmentShaderPath = applicationPath.dirPath() + "TP4/shaders/" + "shadow_mapping_depth.fs.glsl";

    glimac::Program depthProgram = glimac::loadProgram(depthVertexShaderPath, depthFragmentShaderPath);
    if (depthProgram.getGLId() == 0)
    {
        std::cerr << "Failed to load depth shaders" << std::endl;
        return -1;
    }
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

    GLint uMVPMatrixLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uMVPMatrix");
    GLint uMVMatrixLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uMVMatrix");
    GLint uNormalMatrixLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uNormalMatrix");
    GLint uTextureLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uTexture");
    GLint uUseTextureLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uUseTexture");

    GLint uKdLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uKd");
    GLint uKsLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uKs");
    GLint uShininessLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uShininess");
    GLint uLightDir_vsLocation = glGetUniformLocation(unifiedProgram.getGLId(), "uLightDir_vs");
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
    if (uLightDir_vsLocation == -1)
        std::cerr << "Failed to get 'uLightDir_vs' location" << std::endl;
    if (uLightIntensityLocation == -1)
        std::cerr << "Failed to get 'uLightIntensity' location" << std::endl;

    // Load textures
    GLuint textureID = loadTexture(applicationPath.dirPath() + "../TP4/assets/textures_HD/cobblestone_8bit.png");
    GLuint stoneTextureID = loadTexture(applicationPath.dirPath() + "../TP4/assets/textures_HD/stone_8bit.png");
    GLuint brownTerracottaTextureID = loadTexture(applicationPath.dirPath() + "../TP4/assets/textures_HD/brown_glazed_terracotta_8bit.png");

    GLuint soccerTextureID = loadTexture(applicationPath.dirPath() + "../TP4/assets/textures_sphere/soccer_sph_s_8bit.png");

    // load normal maps
    GLuint textureID_normalMap = loadTexture(applicationPath.dirPath() + "../TP4/assets/textures_HD/cobblestone_8bit_normal_map.png");
    GLuint stoneTextureID_normalMap = loadTexture(applicationPath.dirPath() + "../TP4/assets/textures_HD/stone_8bit_normal_map.png");
    GLuint brownTerracottaTextureID_normalMap = loadTexture(applicationPath.dirPath() + "../TP4/assets/textures_HD/brown_glazed_terracotta_8bit_normal_map.png");

    GLuint soccerTextureID_normalMap = loadTexture(applicationPath.dirPath() + "../TP4/assets/textures_sphere/soccer_sph_s_8bit_normal_map.png");

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    std::cout << "Depth test enabled" << std::endl;

    // Set up shadow map framebuffer
    GLuint shadowMapFBO;
    glGenFramebuffers(1, &shadowMapFBO);

    // Check framebuffer completeness
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "Framebuffer not complete: " << status << std::endl;
    }
    else
    {
        std::cout << "Framebuffer setup complete for shadow map" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind after check

    GLuint shadowMap;
    // Generate shadow map texture
    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 8192, 8192, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Set the clear color to a dark gray
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Light properties
    glm::vec3 lightDirWorld = glm::vec3(-1.0f, -1.0f, -1.0f); // Initial light direction

    // Light space matrix for shadows
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 1.0f, 50.0f);
    glm::mat4 lightView = glm::lookAt(glm::vec3(5.0f, 10.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;

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

    // Prepare to add objects to the scene
    GLsizei cubeIndexCount = static_cast<GLsizei>(cubeIndices.size());
    GLsizei sphereVertexCount = sphere.getVertexCount();

    // Add sphere to the scene
    addSphere(
        glm::vec3(0.0f, 0.0f, 0.0f), // Position
        1.0f,                        // Radius
        glm::vec3(1.0f),             // Color (white)
        true,                        // Use texture
        soccerTextureID,             // Texture ID
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
        float time = windowManager.getTime();
        // glm::vec3 lightIntensity = glm::vec3(
        //     (sin(time) + 1.0f) / 2.0f,       // Red oscillates between 0 and 1
        //     (cos(time) + 1.0f) / 2.0f,       // Green oscillates between 0 and 1
        //     (sin(time * 0.5f) + 1.0f) / 2.0f // Blue oscillates more slowly between 0 and 1
        // );

        // static white light
        glm::vec3 lightIntensity = glm::vec3(1.0f);

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

        // Update light direction to rotate around the scene
        float rotationSpeed = 0.5f;
        float angle = time * rotationSpeed;

        lightDirWorld = glm::vec3(
            cos(angle),
            -1.0f,
            sin(angle));

        float lightDistance = 10.0f;
        glm::vec3 lightPos = -lightDistance * lightDirWorld;

        // Recalculate light space matrix
        glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 1.0f, 50.0f);
        glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        // First Pass: Render scene from light's perspective to generate shadow map
        glViewport(0, 0, 8192, 8192); // Match shadow map resolution
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Use the depth shader program
        depthProgram.use();
        glUniformMatrix4fv(uDepth_LightSpaceMatrixLocation, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // Render scene objects to shadow map
        for (const auto &object : sceneObjects)
        {
            glm::mat4 modelMatrix = glm::mat4(1.0f);
            modelMatrix = glm::translate(modelMatrix, object.position);
            if (object.rotationAngle != 0.0f)
            {
                modelMatrix = glm::rotate(modelMatrix, glm::radians(object.rotationAngle), object.rotationAxis);
            }
            modelMatrix = glm::scale(modelMatrix, object.scale);

            // Set model matrix for depth shader
            glUniformMatrix4fv(uDepth_ModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(modelMatrix));

            glBindVertexArray(object.vaoID);
            if (object.type == ObjectType::Cube)
            {
                glDrawElements(GL_TRIANGLES, object.indexCount, GL_UNSIGNED_INT, 0);
            }
            else if (object.type == ObjectType::Sphere)
            {
                glDrawArrays(GL_TRIANGLES, 0, object.indexCount);
            }
            glBindVertexArray(0);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Second Pass: Render the scene normally with shadows
        glViewport(0, 0, window_width, window_height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        unifiedProgram.use();

        // Update light direction to view space
        glm::vec3 lightDirViewSpace = glm::vec3(ViewMatrix * glm::vec4(lightDirWorld, 0.0f));

        // Set light properties
        glUniform3fv(uLightDir_vsLocation, 1, glm::value_ptr(lightDirViewSpace));
        glUniform3fv(uLightIntensityLocation, 1, glm::value_ptr(lightIntensity));

        // Set the updated light space matrix
        glUniformMatrix4fv(glGetUniformLocation(unifiedProgram.getGLId(), "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // Bind the shadow map to texture unit 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glUniform1i(glGetUniformLocation(unifiedProgram.getGLId(), "shadowMap"), 1);

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