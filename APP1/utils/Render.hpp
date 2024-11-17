#ifndef RENDER_HPP
#define RENDER_HPP

#include <glimac/Sphere.hpp>
#include <glad/glad.h>

namespace utils {

    void renderSphere(GLuint vao, const glimac::Sphere& sphere);

} // namespace utils

#endif // RENDER_HPP