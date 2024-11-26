#include "shader.hpp"
#include <iostream>
#include <glimac/FilePath.hpp>

namespace utils_loader {

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    glimac::FilePath vPath(vertexPath.c_str());
    glimac::FilePath fPath(fragmentPath.c_str());

    // Debugging: Print paths
    std::cout << "Loading vertex shader: " << vPath.dirPath() << std::endl;
    std::cout << "Loading fragment shader: " << fPath.dirPath() << std::endl;

    // Load shader program
    m_program = glimac::loadProgram(vPath, fPath);
    if (m_program.getGLId() == 0) {
        std::cerr << "Failed to load shader program: " << vertexPath << " and " << fragmentPath << std::endl;
    }
}

void Shader::use() const {
    m_program.use();
}

GLuint Shader::getID() const {
    return m_program.getGLId();
}

GLint Shader::getUniformLocation(const std::string& name) const {
    return glGetUniformLocation(m_program.getGLId(), name.c_str());
}

GLuint Shader::getGLId() const { // New method implementation
    return m_program.getGLId();
}

} // namespace utils_loader