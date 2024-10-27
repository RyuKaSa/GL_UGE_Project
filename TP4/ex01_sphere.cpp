#define GLFW_INCLUDE_NONE
#include <glad/glad.h>

#include <iostream>
#include <glimac/Sphere.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glimac/Program.hpp>

#include "glimac/SDLWindowManager.hpp"
#include <SDL2/SDL.h>
#include <cstddef> // For offsetof

int window_width = 800;
int window_height = 800;


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

    // Create and bind VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);
    if (vbo == 0) {
        std::cerr << "Failed to generate VBO" << std::endl;
        return -1;
    }
    std::cout << "VBO generated: " << vbo << std::endl;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sphere.getVertexCount() * sizeof(glimac::ShapeVertex), sphere.getDataPointer(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    std::cout << "VBO data buffered" << std::endl;

    // Create VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    if (vao == 0) {
        std::cerr << "Failed to generate VAO" << std::endl;
        return -1;
    }
    std::cout << "VAO generated: " << vao << std::endl;

    // Bind VAO and set up vertex attributes
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

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
    std::cout << "VAO set up" << std::endl;

    // Load shaders
    glimac::FilePath applicationPath(argv[0]);
    std::string vertexShaderPath = applicationPath.dirPath() + "TP4/shaders/" + (argc > 1 ? argv[1] : "sphere.vs.glsl");
    std::string fragmentShaderPath = applicationPath.dirPath() + "TP4/shaders/" + (argc > 2 ? argv[2] : "sphere.fs.glsl");

    std::cout << "Vertex shader path: " << vertexShaderPath << std::endl;
    std::cout << "Fragment shader path: " << fragmentShaderPath << std::endl;

    glimac::Program program = glimac::loadProgram(vertexShaderPath, fragmentShaderPath);
    if (program.getGLId() == 0) {
        std::cerr << "Failed to load shaders" << std::endl;
        return -1;
    }
    std::cout << "Shaders loaded successfully" << std::endl;

    program.use();
    std::cout << "Program in use" << std::endl;

    // Get uniform locations
    GLint uMVPMatrixLocation = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
    GLint uMVMatrixLocation = glGetUniformLocation(program.getGLId(), "uMVMatrix");
    GLint uNormalMatrixLocation = glGetUniformLocation(program.getGLId(), "uNormalMatrix");

    if (uMVPMatrixLocation == -1 || uMVMatrixLocation == -1 || uNormalMatrixLocation == -1) {
        std::cerr << "Failed to get uniform locations" << std::endl;
        return -1;
    }
    std::cout << "Uniform locations retrieved" << std::endl;

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    std::cout << "Depth test enabled" << std::endl;

    // Set the clear color to a dark gray
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Main loop
    bool done = false;
    std::cout << "Entering main loop" << std::endl;
    while (!done) {
        // Clear buffers
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Event handling
        SDL_Event e;
        while (windowManager.pollEvent(e)) {
            if (e.type == SDL_QUIT) {
                done = true;
            }
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_q || e.key.keysym.sym == SDLK_a) {
                    done = true;
                }
            }
        }

        // Get elapsed time
        float time = windowManager.getTime();

        // Define MVP matrices
        // Projection matrix: 70° Field of View, aspect ratio, near and far planes
        glm::mat4 ProjMatrix = glm::perspective(
            glm::radians(70.0f),                            // Field of View
            window_width / (float)window_height,            // Aspect ratio
            0.1f,                                           // Near clipping plane
            100.0f                                          // Far clipping plane
        );

        // View matrix: camera at (0, 0, 5), looking at (0, 0, 0), up vector (0, 1, 0)
        glm::mat4 ViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -5));

        // Model matrix: rotate over time
        glm::mat4 ModelMatrix = glm::rotate(
            glm::mat4(1.0f),
            time,                                           // Rotation angle in radians
            glm::vec3(0, 1, 0)                              // Rotation axis (y-axis)
        );

        // Combined Model-View and Model-View-Projection matrices
        glm::mat4 MVMatrix = ViewMatrix * ModelMatrix;
        glm::mat4 MVPMatrix = ProjMatrix * MVMatrix;

        // Normal matrix (transpose of the inverse of MVMatrix)
        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        // Send matrices to the GPU
        glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
        glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        // Bind VAO and draw the sphere
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        glBindVertexArray(0);

        // Swap buffers
        windowManager.swapBuffers();
    }

    // Clean up
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    std::cout << "Program terminated successfully" << std::endl;

    return 0;
}