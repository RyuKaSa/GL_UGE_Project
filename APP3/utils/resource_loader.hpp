#ifndef RESOURCE_LOADER_HPP
#define RESOURCE_LOADER_HPP

#include "global.hpp"
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>

namespace utils_loader {

// Function to load textures
void loadTextures(GLuint& textureID, GLuint& stoneTextureID, GLuint& brownTerracottaTextureID, GLuint& soccerTextureID,
                  GLuint& textureID_normalMap, GLuint& stoneTextureID_normalMap, GLuint& brownTerracottaTextureID_normalMap, GLuint& soccerTextureID_normalMap,
                  GLuint& chairBaseColorTextureID, GLuint& chairNormalMapTextureID,
                  const glimac::FilePath& applicationPath);

// Function to set up a depth cube map
void setupDepthCubeMap(GLuint& depthCubeMap, GLuint& shadowMapFBO, int resolution = 4096);

} // namespace utils_loader

#endif // RESOURCE_LOADER_HPP