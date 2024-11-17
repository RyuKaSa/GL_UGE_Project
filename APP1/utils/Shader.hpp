// utils/Shader.hpp
#ifndef SHADER_HPP
#define SHADER_HPP

#include <string>
#include <glimac/Program.hpp>
#include <glad/glad.h>

namespace utils {

class Shader {
public:
    // Constructor that builds the shader program from vertex and fragment shader file paths
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    // Activate the shader program
    void use() const;

    // Retrieve the OpenGL ID of the shader program
    GLuint getID() const;

    // Retrieve the location of a uniform variable
    GLint getUniformLocation(const std::string& name) const;

private:
    glimac::Program m_program; // Encapsulates the shader program
};

} // namespace utils

#endif // SHADER_HPP