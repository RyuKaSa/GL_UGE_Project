#include "utilities.hpp"
#include "global.hpp"


#include <glm/glm.hpp>
#include <cstdlib>

#include <iostream>

float randomFloat() {
    return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
}

glm::vec3 randomColor() {
    return glm::vec3(0.6f);
}

AABB computeAABB(const std::vector<float>& vertices) {
    if (vertices.empty()) {
        return AABB(glm::vec3(0.0f), glm::vec3(0.0f));
    }
    
    glm::vec3 minVertex(vertices[0], vertices[1], vertices[2]);
    glm::vec3 maxVertex = minVertex;
    
    for (size_t i = 3; i < vertices.size(); i += 3) {
        glm::vec3 vertex(vertices[i], vertices[i + 1], vertices[i + 2]);
        minVertex = glm::min(minVertex, vertex);
        maxVertex = glm::max(maxVertex, vertex);
    }
    
    return AABB(minVertex, maxVertex);
}

namespace utils_game_loop {

void eventHandler(glimac::SDLWindowManager &windowManager, bool &done, bool &isRockingChairPaused, double &rockingChairStartTime, double &rockingChairPausedTime, float &yaw, float &pitch, glm::vec3 &cameraFront, float &currentFrame) {

    // Event handling
    SDL_Event e;
    while (windowManager.pollEvent(e))
    {
    if (e.type == SDL_QUIT)
        {
            done = true;
        }
        if (e.type == SDL_KEYDOWN)
        {
            if (e.key.keysym.sym == SDLK_ESCAPE)
            {
                done = true;
            }
            else if (e.key.keysym.sym == SDLK_t)
            {
                isRockingChairPaused = !isRockingChairPaused;
                if (isRockingChairPaused)
                {
                    // Record the time when paused
                    rockingChairPausedTime = currentFrame;
                }
                else
                {
                    // Adjust the start time when unpausing
                    double pauseDuration = currentFrame - rockingChairPausedTime;
                    rockingChairStartTime += pauseDuration;
                }
            }
        }

        // Mouse movement
        if (e.type == SDL_MOUSEMOTION)
        {
            float xpos = e.motion.xrel;
            float ypos = e.motion.yrel;

            xpos *= sensitivity;
            ypos *= sensitivity;

            yaw += xpos;
            pitch -= ypos; // Invert y-axis if necessary

            // Constrain pitch
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;

            // Update camera front vector
            glm::vec3 front;
            front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            front.y = sin(glm::radians(pitch));
            front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            cameraFront = glm::normalize(front);
        }
    }
}

} // namespace utils_game_loop