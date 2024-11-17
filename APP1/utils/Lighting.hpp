#ifndef LIGHTING_HPP
#define LIGHTING_HPP

#include <glm/glm.hpp>

namespace utils {

    struct PointLight {
        glm::vec3 position;
        glm::vec3 color;
        float intensity;
    };

} // namespace utils

#endif // LIGHTING_HPP