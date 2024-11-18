// main.cpp
#include <iostream>
#include <vector>
#include <string>

#include "utils/Initialization.hpp"
#include "utils/Shader.hpp"
#include "utils/GBuffer.hpp"
#include "utils/Renderer.hpp"
#include "utils/Camera.hpp"
#include "utils/Lighting.hpp"
#include "utils/Buffer.hpp"
#include "utils/Objects.hpp"

int WINDOW_HEIGHT = 1000;
int WINDOW_WIDTH = 1600;

int main(int argc, char* argv[]) {
    (void)argc;
    glimac::FilePath applicationPath(argv[0]);
    std::cout << "Program started" << std::endl;

    // Initialize SDL and OpenGL
    glimac::SDLWindowManager windowManager(WINDOW_WIDTH, WINDOW_HEIGHT, "Deferred Rendering");
    if (!utils::initializeSDL_GL(WINDOW_WIDTH, WINDOW_HEIGHT, "Deferred Rendering", windowManager)) {
        return -1;
    }

    // Create and compile shaders
    std::cout << "Creating shaders" << std::endl;
    // print current working directory
    std::cout << "Current working directory: " << applicationPath.dirPath() << std::endl;
    utils::Shader gBufferShader(applicationPath.dirPath() + "/../APP1/shaders/deferred_gbuffer.vs.glsl", applicationPath.dirPath() + "/../APP1/shaders/deferred_gbuffer.fs.glsl");
    utils::Shader lightingShader(applicationPath.dirPath() + "/../APP1/shaders/deferred_lighting.vs.glsl", applicationPath.dirPath() + "/../APP1/shaders/deferred_lighting.fs.glsl");

    // Check if shaders compiled successfully
    if (gBufferShader.getID() == 0 || lightingShader.getID() == 0) {
        std::cerr << "Failed to compile/link shaders. Exiting." << std::endl;
        return -1;
    }

    // Create a sphere mesh for all spheres
    glimac::Sphere sphere(1, 32, 16);
    GLuint sphereVBO, sphereVAO;
    utils::createVBOAndVAO(sphereVBO, sphereVAO, sphere);

    // Setup G-buffer
    utils::GBuffer gBuffer(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Create a full-screen quad
    GLuint quadVAO = utils::createFullScreenQuad();

    // Setup point lights
    std::vector<utils::PointLight> pointLights;

    utils::PointLight light1;
    light1.position = glm::vec3(4.0f, 1.0f, 1.0f);
    light1.color = glm::vec3(1.0f, 0.0f, 0.0f); // Red light
    light1.intensity = 3.0f;
    pointLights.push_back(light1);

    utils::PointLight light2;
    light2.position = glm::vec3(4.0f, 1.0f, -1.0f);
    light2.color = glm::vec3(0.0f, 1.0f, 0.0f); // Green light
    light2.intensity = 2.0f;
    pointLights.push_back(light2);

    // create objects here
    // Initialize spheres using the Objects module
    objects::addSphere(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));
    objects::addSphere(glm::vec3(2.0f, 0.0f, 0.0f), 0.5f, glm::vec3(1.0f, 1.0f, 1.0f));

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Initialize camera
    utils::Camera camera({0.0f, 0.0f, 5.0f});
    SDL_SetRelativeMouseMode(SDL_TRUE); // Capture the mouse

    // Renderer instance
    utils::Renderer renderer(WINDOW_WIDTH, WINDOW_HEIGHT);

    bool done = false;
    std::cout << "Entering main loop" << std::endl;
    while (!done) {
        // Calculate delta time
        float deltaTime = utils::calculateDeltaTime(windowManager);

        // Handle input
        done = utils::handleInput(camera, windowManager, deltaTime, 0.1f);

        // Geometry Pass
        glm::mat4 modelMatrix = glm::mat4(1.0f); // Identity matrix (no transformations)
        renderer.geometryPass(camera, sphereVAO, objects::getSpheres(), gBufferShader, sphere.getVertexCount(), WINDOW_WIDTH, WINDOW_HEIGHT);

        // Lighting Pass
        renderer.lightingPass(camera, quadVAO, pointLights, lightingShader);

        // Swap buffers
        windowManager.swapBuffers();
    }

    // Cleanup
    glDeleteBuffers(1, &sphereVBO);
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteVertexArrays(1, &quadVAO);

    std::cout << "Program terminated successfully" << std::endl;
    return 0;
}