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
#include <vector>

#include <glimac/SDLWindowManager.hpp>
#include "utils/Init.hpp"
#include "utils/Render.hpp"
#include "utils/Camera.hpp"
#include "utils/Lighting.hpp"

constexpr int window_width = 800;
constexpr int window_height = 800;
constexpr float mouseSensitivity = 0.1f;

#define NR_POINT_LIGHTS 10

void setupGBuffer(GLuint &gBuffer, GLuint &gPosition, GLuint &gNormal, GLuint &gAlbedoSpec) {
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // Position buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // Normal buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, window_width, window_height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // Albedo + specular buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, window_width, window_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    // Specify the attachments for the framebuffer
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);

    // Depth buffer
    GLuint rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, window_width, window_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    // Check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint createFullScreenQuad() {
    GLuint quadVAO, quadVBO;
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f,  1.0f,
        -1.0f, -1.0f,  0.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  0.0f,

        -1.0f,  1.0f,  0.0f,  1.0f,
         1.0f, -1.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);
    return quadVAO;
}

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

    glimac::SDLWindowManager windowManager(window_width, window_height, "Deferred Rendering");
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
    std::string vertexShaderPath = applicationPath.dirPath() + "APP1/shaders/deferred_gbuffer.vs.glsl";
    std::string fragmentShaderPath = applicationPath.dirPath() + "APP1/shaders/deferred_gbuffer.fs.glsl";
    glimac::Program gBufferProgram = glimac::loadProgram(vertexShaderPath, fragmentShaderPath);
    if (gBufferProgram.getGLId() == 0) {
        std::cerr << "Failed to load gbuffer shaders" << std::endl;
        return -1;
    }

    std::string lightingVertexShaderPath = applicationPath.dirPath() + "APP1/shaders/deferred_lighting.vs.glsl";
    std::string lightingFragmentShaderPath = applicationPath.dirPath() + "APP1/shaders/deferred_lighting.fs.glsl";
    glimac::Program lightingProgram = glimac::loadProgram(lightingVertexShaderPath, lightingFragmentShaderPath);
    if (lightingProgram.getGLId() == 0) {
        std::cerr << "Failed to load lighting shaders" << std::endl;
        return -1;
    }

    glm::mat4 ModelMatrix = glm::mat4(1.0f);

    gBufferProgram.use();

    GLint uMVPMatrixLocation = glGetUniformLocation(gBufferProgram.getGLId(), "uMVPMatrix");
    GLint uNormalMatrixLocation = glGetUniformLocation(gBufferProgram.getGLId(), "uNormalMatrix");
    GLint uModelMatrixLocation = glGetUniformLocation(gBufferProgram.getGLId(), "uModelMatrix");

    // Debugging Output
    std::cout << "Uniform Locations gBuffer:\n";
    std::cout << "uMVPMatrix: " << uMVPMatrixLocation << "\n";
    std::cout << "uNormalMatrix: " << uNormalMatrixLocation << "\n";
    std::cout << "uModelMatrix: " << uModelMatrixLocation << "\n";

    if (uMVPMatrixLocation == -1 || uNormalMatrixLocation == -1 || uModelMatrixLocation == -1) {
        std::cerr << "One or more uniform locations could not be found. Exiting." << std::endl;
        return -1;
    }

    // If all uniform locations are found, set any required initial values.
    if (uModelMatrixLocation != -1) {
        glUniformMatrix4fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
    }

    // Setup G-Buffer
    GLuint gBuffer, gPosition, gNormal, gAlbedoSpec;
    setupGBuffer(gBuffer, gPosition, gNormal, gAlbedoSpec);

    // Full-screen quad for lighting pass
    GLuint quadVAO = createFullScreenQuad();

    // Setup point lights
    std::vector<utils::PointLight> pointLights;
    utils::PointLight light1;
    light1.position = glm::vec3(2.0f, 2.0f, 2.0f);
    light1.color = glm::vec3(1.0f, 0.0f, 0.0f); // Red light
    light1.intensity = 1.0f;
    pointLights.push_back(light1);

    utils::PointLight light2;
    light2.position = glm::vec3(-2.0f, 2.0f, 2.0f);
    light2.color = glm::vec3(0.0f, 1.0f, 0.0f); // Green light
    light2.intensity = 1.0f;
    pointLights.push_back(light2);

    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    utils::Camera camera({0.0f, 0.0f, 5.0f});
    SDL_SetRelativeMouseMode(SDL_TRUE);

    float fps = 0.0f;
    static float fpsTimer = 0.0f; // Accumulate time
    static int frameCount = 0;    // Count frames

    bool done = false;
    std::cout << "Entering main loop" << std::endl;
    while (!done) {
        float deltaTime = utils::calculateDeltaTime(windowManager);

        // Increment frame count and accumulate time
        fpsTimer += deltaTime;
        frameCount++;

        // Update FPS display once every second
        if (fpsTimer >= 1.0f) {
            int fps = static_cast<int>(frameCount); // Calculate FPS as an integer
            frameCount = 0;   // Reset frame count
            fpsTimer -= 1.0f; // Reset timer (subtract 1.0 to handle any excess time)

            // Update window title with new FPS
            std::string newTitle = "FPS: " + std::to_string(fps);
            SDL_SetWindowTitle(windowManager.getWindow(), newTitle.c_str());
}

        // Geometry Pass
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gBufferProgram.use();
        {
            glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.0f), window_width / (float)window_height, 0.1f, 100.0f);
            glm::mat4 ViewMatrix = camera.getViewMatrix();
            glm::mat4 ModelMatrix = glm::mat4(1.0f);

            glm::mat4 MVPMatrix = ProjMatrix * ViewMatrix * ModelMatrix;
            glm::mat4 NormalMatrix = glm::transpose(glm::inverse(ModelMatrix));

            glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
            glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
            glUniformMatrix4fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(ModelMatrix));

            utils::renderSphere(vao, sphere);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Lighting Pass
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        lightingProgram.use();

        // Set the lighting uniforms
        GLint uKsLocation = glGetUniformLocation(lightingProgram.getGLId(), "uKs");
        GLint uShininessLocation = glGetUniformLocation(lightingProgram.getGLId(), "uShininess");
        GLint uCameraPosLocation = glGetUniformLocation(lightingProgram.getGLId(), "cameraPosWorld");

        if (uKsLocation != -1) {
            glUniform3f(uKsLocation, 0.5f, 0.5f, 0.5f); // Example value for Ks (specular reflection coefficient)
        } else {
            std::cerr << "Uniform uKs not found!" << std::endl;
        }

        if (uShininessLocation != -1) {
            glUniform1f(uShininessLocation, 32.0f); // Example value for shininess
        } else {
            std::cerr << "Uniform uShininess not found!" << std::endl;
        }

        // Passing camera position in world space
        if (uCameraPosLocation != -1) {
            glm::vec3 cameraPos = camera.getPosition(); // Ensure this returns world space position
            glUniform3fv(uCameraPosLocation, 1, glm::value_ptr(cameraPos));
        } else {
            std::cerr << "Uniform cameraPosWorld not found!" << std::endl;
        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glUniform1i(glGetUniformLocation(lightingProgram.getGLId(), "gPosition"), 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glUniform1i(glGetUniformLocation(lightingProgram.getGLId(), "gNormal"), 1);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glUniform1i(glGetUniformLocation(lightingProgram.getGLId(), "gAlbedoSpec"), 2);

        // Set number of point lights
        glUniform1i(glGetUniformLocation(lightingProgram.getGLId(), "numPointLights"), pointLights.size());

        // Set point lights uniforms
        for (int i = 0; i < pointLights.size(); ++i) {
            std::string baseName = "pointLights[" + std::to_string(i) + "]";
            std::string positionName = baseName + ".position";
            std::string colorName = baseName + ".color";
            std::string intensityName = baseName + ".intensity";

            GLint positionLoc = glGetUniformLocation(lightingProgram.getGLId(), positionName.c_str());
            GLint colorLoc = glGetUniformLocation(lightingProgram.getGLId(), colorName.c_str());
            GLint intensityLoc = glGetUniformLocation(lightingProgram.getGLId(), intensityName.c_str());

            if (positionLoc != -1 && colorLoc != -1 && intensityLoc != -1) {
                glUniform3fv(positionLoc, 1, glm::value_ptr(pointLights[i].position));
                glUniform3fv(colorLoc, 1, glm::value_ptr(pointLights[i].color));
                glUniform1f(intensityLoc, pointLights[i].intensity);
            } else {
                std::cerr << "Failed to get uniform locations for pointLights[" << i << "]" << std::endl;
            }
        }

        // Draw the quad
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        // Handle input and swap buffers
        windowManager.swapBuffers();
        done = utils::handleInput(camera, windowManager, deltaTime, mouseSensitivity);
    }

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
    std::cout << "Program terminated successfully" << std::endl;

    return 0;
}