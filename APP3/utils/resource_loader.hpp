#ifndef RESOURCE_LOADER_HPP
#define RESOURCE_LOADER_HPP

#include "global.hpp"
#include <glimac/Program.hpp>
#include <glimac/FilePath.hpp>

namespace utils_loader {

extern std::vector<GLuint> allTextures;

// Function to load textures
void loadTextures(GLuint& textureID, GLuint& stoneTextureID, GLuint& brownTerracottaTextureID, GLuint& soccerTextureID,
                  GLuint& textureID_normalMap, GLuint& stoneTextureID_normalMap, GLuint& brownTerracottaTextureID_normalMap, GLuint& soccerTextureID_normalMap,
                  GLuint& chairBaseColorTextureID, GLuint& chairNormalMapTextureID,
                  GLuint& texture_ID_deepslate_emerald_ore, GLuint& texture_ID_deepslate_emerald_ore_n, GLuint& texture_ID_deepslate_emerald_ore_s,
                  GLuint& texture_ID_glass, GLuint& texture_ID_purple_stained_glass,
                  GLuint& testure_ID_iron_block, GLuint& texture_ID_iron_block_n, GLuint& texture_ID_iron_block_s,
                  GLuint& texture_ID_note_block, GLuint& texture_ID_note_block_n, GLuint& texture_ID_note_block_s,
                  GLuint& texture_ID_oak_planks, GLuint& texture_ID_oak_planks_n, GLuint& texture_ID_oak_planks_s,
                  GLuint& texture_ID_stone_bricks, GLuint& texture_ID_stone_bricks_n, GLuint& texture_ID_stone_bricks_s,
                  GLuint& skyboxTextureID,
                  GLuint& sunTextureID, GLuint& mercuryTextureID, GLuint& venusTextureID, GLuint& venus_atmosphereTextureID,
                  GLuint& marsTextureID, GLuint& jupiterTextureID, GLuint& saturnTextureID, GLuint& saturn_ringTextureID,
                  GLuint& uranusTextureID, GLuint& neptuneTextureID,
                  GLuint& earthTextureID, GLuint& earth_nTextureID, GLuint& earth_sTextureID, GLuint& earth_atmosphereTextureID,
                  const glimac::FilePath& applicationPath);

// Function to set up a depth cube map
void setupDepthCubeMap(GLuint& depthCubeMap, GLuint& shadowMapFBO, int resolution = 4096);

} // namespace utils_loader

#endif // RESOURCE_LOADER_HPP