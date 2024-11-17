#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "glimac/SDLWindowManager.hpp"

namespace utils {

    constexpr float cameraSpeed = 5.0f;

    class Camera {
    public:
        explicit Camera(const glm::vec3& initialPosition);
        glm::mat4 getViewMatrix() const;
        void processKeyboard(const Uint8* state, float deltaTime);
        void processMouseMovement(float xOffset, float yOffset, float sensitivity);

        glm::vec3 getPosition() const { return cameraPos; }

    private:
        glm::vec3 cameraPos;
        glm::vec3 cameraFront;
        glm::vec3 cameraUp;
        float yaw;
        float pitch;
        const float initialYPos;
    };

    bool handleInput(Camera& camera, glimac::SDLWindowManager& windowManager, float deltaTime, float sensitivity);
    float calculateDeltaTime(glimac::SDLWindowManager& windowManager);

} // namespace utils

#endif // CAMERA_HPP