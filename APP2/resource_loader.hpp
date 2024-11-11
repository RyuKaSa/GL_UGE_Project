#ifndef RESOURCE_LOADER_HPP
#define RESOURCE_LOADER_HPP

#include "global.hpp"
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>

// Function to load shaders
glimac::Program loadUnifiedShader(const glimac::FilePath& applicationPath);
glimac::Program loadDepthShader(const glimac::FilePath& applicationPath);

// Function to load textures
void loadTextures(GLuint& textureID, GLuint& stoneTextureID, GLuint& brownTerracottaTextureID, GLuint& soccerTextureID,
                  GLuint& textureID_normalMap, GLuint& stoneTextureID_normalMap, GLuint& brownTerracottaTextureID_normalMap, GLuint& soccerTextureID_normalMap, const glimac::FilePath& applicationPath);

// Function to set up a depth cube map
void setupDepthCubeMap(GLuint& depthCubeMap, GLuint& shadowMapFBO, int resolution = 4096);

#endif // RESOURCE_LOADER_HPP