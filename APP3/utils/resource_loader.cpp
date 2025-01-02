#include "resource_loader.hpp"
#include "texture.hpp"
#include <glimac/Image.hpp>
#include <iostream>

#include <cstdlib>
#include <ctime>

namespace utils_loader {

// Load textures
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
                  const glimac::FilePath& applicationPath) {
    textureID = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_HD/cobblestone_8bit.png");
    stoneTextureID = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_HD/stone_8bit.png");
    brownTerracottaTextureID = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_HD/brown_glazed_terracotta_8bit.png");
    soccerTextureID = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_sphere/soccer_sph_s_8bit.png");
    chairBaseColorTextureID = loadTexture(applicationPath.dirPath() + "../assets/models/Rocking_Chair/Textures/BaseColor.png");

    textureID_normalMap = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_HD/cobblestone_8bit_normal_map.png");
    stoneTextureID_normalMap = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_HD/stone_8bit_normal_map.png");
    brownTerracottaTextureID_normalMap = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_HD/brown_glazed_terracotta_8bit_normal_map.png");
    soccerTextureID_normalMap = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_sphere/soccer_sph_s_8bit_normal_map.png");
    chairNormalMapTextureID = loadTexture(applicationPath.dirPath() + "../assets/models/Rocking_Chair/Textures/Normal.png");

    // final textures
    // deepslate_emerald_ore
    texture_ID_deepslate_emerald_ore = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/deepslate_emerald_ore/deepslate_emerald_ore.png");
    texture_ID_deepslate_emerald_ore_n = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/deepslate_emerald_ore/deepslate_emerald_ore_n.png");
    texture_ID_deepslate_emerald_ore_s = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/deepslate_emerald_ore/deepslate_emerald_ore_s2.png");

    // glass
    texture_ID_glass = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/glass/glass.png");
    texture_ID_purple_stained_glass = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/glass/purple_stained_glass.png");

    // iron_block
    testure_ID_iron_block = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/iron_block/iron_block.png");
    texture_ID_iron_block_n = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/iron_block/iron_block_n.png");
    texture_ID_iron_block_s = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/iron_block/iron_block_s.png");

    // note_block
    texture_ID_note_block = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/note_block/note_block.png");
    texture_ID_note_block_n = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/note_block/note_block_n.png");
    texture_ID_note_block_s = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/note_block/note_block_s.png");

    // oak_planks
    texture_ID_oak_planks = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/oak_planks/oak_planks.png");
    texture_ID_oak_planks_n = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/oak_planks/oak_planks_n.png");
    texture_ID_oak_planks_s = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/oak_planks/oak_planks_s.png");

    // stone_bricks
    texture_ID_stone_bricks = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/stone_bricks/stone_bricks.png");
    texture_ID_stone_bricks_n = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/stone_bricks/stone_bricks_n.png");
    texture_ID_stone_bricks_s = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/stone_bricks/stone_bricks_s.png");

    // skyboxTextureID = loadTexture(applicationPath.dirPath() + "../APP3/assets/skybox/skybox.png");
    // we can randomly chosoe at start one of the skyboxes, skybox1 to skybox4, but we need to load the 4 skyboxes
    GLuint sky1, sky2, sky3, sky4;

    sky1 = loadTexture(applicationPath.dirPath() + "../APP3/assets/skybox/1.png");
    sky2 = loadTexture(applicationPath.dirPath() + "../APP3/assets/skybox/2.png");
    sky3 = loadTexture(applicationPath.dirPath() + "../APP3/assets/skybox/3.png");
    sky4 = loadTexture(applicationPath.dirPath() + "../APP3/assets/skybox/4.png");

    // randomly choose one of the variables sky1 to sky4 for skyboxTextureID
    srand(static_cast<unsigned int>(time(0)));

    int random = rand() % 4 + 1;

    switch (random) {
    case 1:
        skyboxTextureID = sky1;
        break;
    case 2:
        skyboxTextureID = sky2;
        break;
    case 3:
        skyboxTextureID = sky3;
        break;
    case 4:
        skyboxTextureID = sky4;
        break;
    default:
        skyboxTextureID = sky1;
        break;
    }

    // print the skybox file
    std::cout << "Skybox file: " << random << ".png" << std::endl;

    std::cout << "Textures loaded successfully" << std::endl;

    // check textures and normals fir the chair : 
    std::cout << "Chair Base Color Texture ID: " << chairBaseColorTextureID << std::endl;
    std::cout << "Chair Normal Map Texture ID: " << chairNormalMapTextureID << std::endl;
}

// Setup depth cube map
void setupDepthCubeMap(GLuint& depthCubeMap, GLuint& shadowMapFBO, int resolution) {
    glGenTextures(1, &depthCubeMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
    for (unsigned int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT32F, resolution, resolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &shadowMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::cout << "Depth cube map setup complete" << std::endl;
}

} // namespace utils_loader