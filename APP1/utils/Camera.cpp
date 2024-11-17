#include "Camera.hpp"

namespace utils {

    Camera::Camera(const glm::vec3& initialPosition)
        : cameraPos(initialPosition), cameraFront(0.0f, 0.0f, -1.0f), cameraUp(0.0f, 1.0f, 0.0f),
          yaw(-90.0f), pitch(0.0f), initialYPos(initialPosition.y) {}

    glm::mat4 Camera::getViewMatrix() const {
        return glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    }

    void Camera::processKeyboard(const Uint8* state, float deltaTime) {
        glm::vec3 frontDirection = glm::normalize(glm::vec3(cameraFront.x, 0.0f, cameraFront.z));
        glm::vec3 rightDirection = glm::normalize(glm::cross(frontDirection, cameraUp));
        float speed = deltaTime * cameraSpeed;

        if (state[SDL_SCANCODE_W]) cameraPos += speed * frontDirection;
        if (state[SDL_SCANCODE_S]) cameraPos -= speed * frontDirection;
        if (state[SDL_SCANCODE_A]) cameraPos -= speed * rightDirection;
        if (state[SDL_SCANCODE_D]) cameraPos += speed * rightDirection;

        cameraPos.y = initialYPos;
    }

    void Camera::processMouseMovement(float xOffset, float yOffset, float sensitivity) {
        yaw += xOffset * sensitivity;
        pitch -= yOffset * sensitivity;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }

    bool handleInput(Camera& camera, glimac::SDLWindowManager& windowManager, float deltaTime, float sensitivity) {
        SDL_Event e;
        while (windowManager.pollEvent(e)) {
            if (e.type == SDL_QUIT) return true;
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) return true;
            if (e.type == SDL_MOUSEMOTION) {
                camera.processMouseMovement(e.motion.xrel, e.motion.yrel, sensitivity);
            }
        }
        const Uint8* state = SDL_GetKeyboardState(NULL);
        camera.processKeyboard(state, deltaTime);
        return false;
    }

    float calculateDeltaTime(glimac::SDLWindowManager& windowManager) {
        static float lastFrame = 0.0f;
        float currentFrame = windowManager.getTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        return deltaTime;
    }

} // namespace utils