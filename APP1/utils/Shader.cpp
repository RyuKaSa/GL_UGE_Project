// utils/Shader.cpp
#include "Shader.hpp"
#include <glimac/FilePath.hpp>
#include <iostream>

namespace utils {

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    glimac::FilePath vPath(vertexPath.c_str());
    glimac::FilePath fPath(fragmentPath.c_str());
    // print the path attempted to be loaded
    std::cout << "Vertex path: " << vPath.dirPath() << std::endl;
    std::cout << "Fragment path: " << fPath.dirPath() << std::endl;
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

} // namespace utils