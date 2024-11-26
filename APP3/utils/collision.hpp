#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "utilities.hpp"

bool checkCollision(const glm::vec3& cylinderBaseCenter, float radius, float height, const AABB& box);

#endif // COLLISION_HPP