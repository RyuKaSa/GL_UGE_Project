#include "collision.hpp"

bool checkCollision(const glm::vec3& cylinderBaseCenter, float radius, float height, const AABB& box) {
    float horizontalDistanceSquared = 0.0f;

    for (int i = 0; i < 3; i += 2) {
        if (cylinderBaseCenter[i] < box.min[i]) {
            horizontalDistanceSquared += (box.min[i] - cylinderBaseCenter[i]) * (box.min[i] - cylinderBaseCenter[i]);
        } else if (cylinderBaseCenter[i] > box.max[i]) {
            horizontalDistanceSquared += (cylinderBaseCenter[i] - box.max[i]) * (cylinderBaseCenter[i] - box.max[i]);
        }
    }

    if (horizontalDistanceSquared > (radius * radius)) {
        return false;
    }

    float cylinderTop = cylinderBaseCenter.y + height / 2.0f;
    float cylinderBottom = cylinderBaseCenter.y - height / 2.0f;
    if (cylinderTop < box.min.y || cylinderBottom > box.max.y) {
        return false;
    }

    return true;
}