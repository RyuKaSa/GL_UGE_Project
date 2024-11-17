#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <iostream>
#include <glimac/Sphere.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glimac/Program.hpp>
#include <SDL2/SDL.h>
#include <cstddef> // For offsetof
#include <glimac/FilePath.hpp>

#include "glimac/SDLWindowManager.hpp"
#include "utils/Init.hpp"
#include "utils/Render.hpp"
#include "utils/Camera.hpp"

constexpr int window_width = 800;
constexpr int window_height = 800;
constexpr float mouseSensitivity = 0.1f;

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;
    std::cout << "Program started" << std::endl;

    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
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

    glimac::SDLWindowManager windowManager(window_width, window_height, "Boules");
    std::cout << "SDLWindowManager initialized" << std::endl;

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
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

    glimac::Sphere sphere(1, 32, 16);
    GLuint vbo, vao;
    utils::createVBOAndVAO(vbo, vao, sphere);

    glimac::FilePath applicationPath(argv[0]);
    std::string vertexShaderPath = applicationPath.dirPath() + "APP1/shaders/" + (argc > 1 ? argv[1] : "sphere.vs.glsl");
    std::string fragmentShaderPath = applicationPath.dirPath() + "APP1/shaders/" + (argc > 2 ? argv[2] : "sphere.fs.glsl");

    glimac::Program program = glimac::loadProgram(vertexShaderPath, fragmentShaderPath);
    if (program.getGLId() == 0) {
        std::cerr << "Failed to load shaders" << std::endl;
        return -1;
    }

    program.use();
    GLint uMVPMatrixLocation = glGetUniformLocation(program.getGLId(), "uMVPMatrix");
    GLint uMVMatrixLocation = glGetUniformLocation(program.getGLId(), "uMVMatrix");
    GLint uNormalMatrixLocation = glGetUniformLocation(program.getGLId(), "uNormalMatrix");
    if (uMVPMatrixLocation == -1 || uMVMatrixLocation == -1 || uNormalMatrixLocation == -1) {
        std::cerr << "Failed to get uniform locations" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    utils::Camera camera({0.0f, 0.0f, 5.0f});
    SDL_SetRelativeMouseMode(SDL_TRUE);

    bool done = false;
    std::cout << "Entering main loop" << std::endl;
    while (!done) {
        float deltaTime = utils::calculateDeltaTime(windowManager);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        done = utils::handleInput(camera, windowManager, deltaTime, mouseSensitivity);

        glm::mat4 ProjMatrix = glm::perspective(
            glm::radians(70.0f), window_width / (float)window_height, 0.1f, 100.0f);
        glm::mat4 ViewMatrix = camera.getViewMatrix();
        glm::mat4 ModelMatrix = glm::mat4(1.0f);

        glm::mat4 MVMatrix = ViewMatrix * ModelMatrix;
        glm::mat4 MVPMatrix = ProjMatrix * MVMatrix;
        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(MVMatrix));

        glUniformMatrix4fv(uMVMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVMatrix));
        glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
        glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

        utils::renderSphere(vao, sphere);

        windowManager.swapBuffers();
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    std::cout << "Program terminated successfully" << std::endl;

    return 0;
}