#ifndef RENDERING_HPP
#define RENDERING_HPP

#include <glm/glm.hpp>
#include <vector>
#include "utils/scene_object.hpp"
#include "utils/utilities.hpp"
#include "utils/scene_object.hpp"
#include "utils/models.hpp"
#include <glimac/SDLWindowManager.hpp>

namespace utils_game_loop {

struct RotationParams {
    glm::vec3 axis;        // Rotation axis
    float speedDegrees;    // Rotation speed in degrees per second
    float currentAngle;    // Current rotation angle in degrees
};

// Initialize rotation parameters for the skybox
static RotationParams skyboxRotation = {
    glm::normalize(glm::vec3(0.1f, 0.1f, 1.0f)), // Slightly off Z-axis for natural rotation
    0.5f,                                       // 15 degrees per second
    150.0f                                       // Starting at 150 degrees
};

void dynamic_loop(float &deltaTime, float &lastFrame, float &currentFrame,
                  glimac::SDLWindowManager &windowManager, glm::vec3 &cameraPos,
                  glm::vec3 &cameraFront, glm::vec3 &cameraUp, float cameraSpeed,
                  bool &done, bool &isRockingChairPaused, double &rockingChairStartTime,
                  double &rockingChairPausedTime, float &yaw, float &pitch, float cameraRadius,
                  double frequency, double radius, double length,
                  float cameraHeight);



} // namespace utils_game_loop

#endif // RENDERING_HPP