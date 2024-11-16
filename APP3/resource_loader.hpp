#ifndef RESOURCE_LOADER_HPP
#define RESOURCE_LOADER_HPP

// #include "global.hpp"
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>
#include <map>

extern std::map<std::string, GLuint> textures;


// Function to load shaders
glimac::Program loadRoom_1Shader(const glimac::FilePath& applicationPath);
glimac::Program loadRoom_2Shader(const glimac::FilePath& applicationPath);
glimac::Program loadDepthShader(const glimac::FilePath& applicationPath);
glimac::Program loadLightingShader(const glimac::FilePath& applicationPath);

GLuint loadTexture(const std::string& texturePath);

// Function to load textures
void loadTextures(
    GLuint& brownTerracottaTextureID,
    GLuint& brownTerracottaTextureID_normalMap,
    GLuint& chairBaseColorTextureID,
    GLuint& chairNormalMapTextureID,
    const glimac::FilePath& applicationPath
);

#endif // RESOURCE_LOADER_HPP