#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <glm/glm.hpp>
#include <vector>
// #include <SDL2/SDL.h>
#include <glimac/SDLWindowManager.hpp>

namespace glimac {
    class SDLWindowManager;
}

struct AABB {
    glm::vec3 min;
    glm::vec3 max;
    AABB() : min(glm::vec3(0.0f)), max(glm::vec3(0.0f)) {}
    AABB(const glm::vec3& min_, const glm::vec3& max_) : min(min_), max(max_) {}
};

float randomFloat();
glm::vec3 randomColor();
AABB computeAABB(const std::vector<float>& vertices);

namespace utils_game_loop {
    void eventHandler(glimac::SDLWindowManager &windowManager, bool &done, bool &isRockingChairPaused, double &rockingChairStartTime, double &rockingChairPausedTime, float &yaw, float &pitch, glm::vec3 &cameraFront, float &currentFrame);
}

#endif // UTILITIES_HPP