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

void dynamic_loop(float &deltaTime, float &lastFrame, float &currentFrame,
                  glimac::SDLWindowManager &windowManager, glm::vec3 &cameraPos,
                  glm::vec3 &cameraFront, glm::vec3 &cameraUp, float cameraSpeed,
                  bool &done, bool &isRockingChairPaused, double &rockingChairStartTime,
                  double &rockingChairPausedTime, float &yaw, float &pitch, float cameraRadius,
                  double frequency, double radius, double length,
                  float cameraHeight);



} // namespace utils_game_loop

#endif // RENDERING_HPP