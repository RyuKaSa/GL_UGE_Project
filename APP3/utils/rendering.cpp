#include "rendering.hpp"

namespace utils_game_loop {

void dynamic_loop(float &deltaTime, float &lastFrame, float &currentFrame,
                  glimac::SDLWindowManager &windowManager, glm::vec3 &cameraPos,
                  glm::vec3 &cameraFront, glm::vec3 &cameraUp, float cameraSpeed,
                  bool &done, bool &isRockingChairPaused, double &rockingChairStartTime,
                  double &rockingChairPausedTime, float &yaw, float &pitch, float cameraRadius,
                  double frequency, double radius, double length,
                  float cameraHeight) {

    // Update dynamic only objects before rendering
    for (auto& object : utils_scene::sceneObjects) {
        if (!object.isStatic) {
            // we place here all objects that we want to move
            if (object.name == "rocking_chair") {
                double adjustedTime = currentFrame - rockingChairStartTime;
                if (isRockingChairPaused) {
                    adjustedTime = rockingChairPausedTime - rockingChairStartTime;
                }

                glm::vec3 offsetPosition;
                glm::vec3 rotation;
                float rotationAngleRadians;
                utils_object::GetRockingChairPositionAndRotation(
                    adjustedTime,
                    frequency,
                    radius,
                    length,
                    offsetPosition,
                    rotation
                );

                // Update position and rotation
                object.position = object.initialPosition + offsetPosition;

                object.rotationAngle = rotation.z; // Rotation around Z-axis
                object.rotationAxis = glm::vec3(1.0f, 0.0f, 0.0f); // Rotation around X-axis
            }
            // if (object.name == "") {
                
            // }
        }
    }

    // Update dynamic objects
    for (auto& object : utils_scene::sceneObjectsSkybox) {
        if (!object.isStatic) {
            if (object.name == "sky") {
                // Keep skybox centered on the camera
                object.position = cameraPos;

                // Increment the rotation angle based on speed and deltaTime
                skyboxRotation.currentAngle += skyboxRotation.speedDegrees * deltaTime;

                // Wrap the angle within [0, 360) degrees
                if (skyboxRotation.currentAngle >= 360.0f) {
                    skyboxRotation.currentAngle -= 360.0f;
                }

                // Apply the updated rotation angle to the object
                object.rotationAngle = skyboxRotation.currentAngle;
                object.rotationAxis = skyboxRotation.axis;

                // Debug output
                // std::cout << "Skybox rotation angle: " << object.rotationAngle << " degrees" << std::endl;
            }
        }
    }
}

} // namespace utils_game_loop