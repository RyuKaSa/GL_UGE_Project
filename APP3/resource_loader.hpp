#ifndef RESOURCE_LOADER_HPP
#define RESOURCE_LOADER_HPP

#include "global.hpp"
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>

extern std::map<std::string, GLuint> textures;


// Function to load shaders
glimac::Program loadRoom_1Shader(const glimac::FilePath& applicationPath);
glimac::Program loadRoom_2Shader(const glimac::FilePath& applicationPath);
glimac::Program loadDepthShader(const glimac::FilePath& applicationPath);

GLuint loadTexture(const std::string& texturePath);

// Function to load textures
void loadTextures(GLuint& textureID, GLuint& stoneTextureID, GLuint& brownTerracottaTextureID, GLuint& soccerTextureID,
                  GLuint& textureID_normalMap, GLuint& stoneTextureID_normalMap, GLuint& brownTerracottaTextureID_normalMap, GLuint& soccerTextureID_normalMap,
                  GLuint& chairBaseColorTextureID, GLuint& chairNormalMapTextureID,
                  const glimac::FilePath& applicationPath);

#endif // RESOURCE_LOADER_HPP