#ifndef SHADER_HPP
#define SHADER_HPP

#include <glimac/Program.hpp>
#include <string>

namespace utils_loader {

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    void use() const;
    GLuint getID() const;
    GLint getUniformLocation(const std::string& name) const;
    GLuint getGLId() const;

private:
    glimac::Program m_program;
};

} // namespace utils_loader

#endif // SHADER_HPP