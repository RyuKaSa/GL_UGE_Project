#include "resource_loader.hpp"
#include "texture.hpp"
#include <glimac/Image.hpp>
#include <iostream>

#include <cstdlib>
#include <ctime>

namespace utils_loader {

std::vector<GLuint> allTextures;

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
                  GLuint& sunTextureID, GLuint& mercuryTextureID, GLuint& venusTextureID, GLuint& venus_atmosphereTextureID,
                  GLuint& marsTextureID, GLuint& jupiterTextureID, GLuint& saturnTextureID, GLuint& saturn_ringTextureID,
                  GLuint& uranusTextureID, GLuint& neptuneTextureID,
                  GLuint& earthTextureID, GLuint& earth_nTextureID, GLuint& earth_sTextureID, GLuint& earth_atmosphereTextureID,
                  GLuint& crying_obsidianTextureID, GLuint& crying_obsidianTextureID_n, GLuint& crying_obsidianTextureID_s,
                  GLuint& deepslateTextureID, GLuint& deepslateTextureID_n, GLuint& deepslateTextureID_s,
                  GLuint& mossy_stone_bricksTextureID, GLuint& mossy_stone_bricksTextureID_n, GLuint& mossy_stone_bricksTextureID_s,
                  GLuint& obsidianTextureID, GLuint& obsidianTextureID_n, GLuint& obsidianTextureID_s,
                  GLuint& portalTextureID, GLuint& portalTextureID_s,
                  
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

    // planets in order

    // sun
    sunTextureID = loadTextureBall(applicationPath.dirPath() + "../APP3/assets/planets/sun/sun.png");

    // mercury
    mercuryTextureID = loadTextureBall(applicationPath.dirPath() + "../APP3/assets/planets/mercury/mercury.png");

    // venus
    venusTextureID = loadTextureBall(applicationPath.dirPath() + "../APP3/assets/planets/venus/surface.png");
    // venus atmosphere
    venus_atmosphereTextureID = loadTextureBall(applicationPath.dirPath() + "../APP3/assets/planets/venus/atmosphere.png");

    // earth
    earthTextureID = loadTextureBall(applicationPath.dirPath() + "../APP3/assets/planets/earth/earth.png");
    earth_nTextureID = loadTextureBall(applicationPath.dirPath() + "../APP3/assets/planets/earth/earth_n.png");
    earth_sTextureID = loadTextureBall(applicationPath.dirPath() + "../APP3/assets/planets/earth/earth_s.png");
    // earth atmosphere
    earth_atmosphereTextureID = loadTextureBall(applicationPath.dirPath() + "../APP3/assets/planets/earth/atmosphere.png");

    // mars
    marsTextureID = loadTextureBall(applicationPath.dirPath() + "../APP3/assets/planets/mars/mars.png");

    // jupiter
    jupiterTextureID = loadTextureBall(applicationPath.dirPath() + "../APP3/assets/planets/jupiter/jupiter.png");

    // saturn
    saturnTextureID = loadTextureBall(applicationPath.dirPath() + "../APP3/assets/planets/saturn/saturn.png");
    // saturn ring
    saturn_ringTextureID = loadTextureBall(applicationPath.dirPath() + "../APP3/assets/planets/saturn/rings.png");

    // uranus
    uranusTextureID = loadTextureBall(applicationPath.dirPath() + "../APP3/assets/planets/uranus/uranus.png");

    // neptune
    neptuneTextureID = loadTextureBall(applicationPath.dirPath() + "../APP3/assets/planets/neptune/neptune.png");

    // crying obs
    crying_obsidianTextureID = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/crying_obsidian/crying_obsidian.png");
    crying_obsidianTextureID_n = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/crying_obsidian/crying_obsidian_n.png");
    crying_obsidianTextureID_s = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/crying_obsidian/crying_obsidian_s.png");

    // deepslate
    deepslateTextureID = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/deepslate/deepslate.png");
    deepslateTextureID_n = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/deepslate/deepslate_n.png");
    deepslateTextureID_s = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/deepslate/deepslate_s.png");

    // mossy stone bricks
    mossy_stone_bricksTextureID = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/mossy_stone_bricks/mossy_stone_bricks.png");
    mossy_stone_bricksTextureID_n = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/mossy_stone_bricks/mossy_stone_bricks_n.png");
    mossy_stone_bricksTextureID_s = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/mossy_stone_bricks/mossy_stone_bricks_s.png");

    // obsidian
    obsidianTextureID = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/obsidian/obsidian.png");
    obsidianTextureID_n = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/obsidian/obsidian_n.png");
    obsidianTextureID_s = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/obsidian/obsidian_s.png");

    // portal
    portalTextureID = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/portal/portal.png");
    portalTextureID_s = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures_cube_PBR/portal/portal_s.png");

    // add all textures to the allTextures vector
    allTextures.push_back(textureID);
    allTextures.push_back(stoneTextureID);
    allTextures.push_back(brownTerracottaTextureID);
    allTextures.push_back(soccerTextureID);
    allTextures.push_back(textureID_normalMap);
    allTextures.push_back(stoneTextureID_normalMap);
    allTextures.push_back(brownTerracottaTextureID_normalMap);
    allTextures.push_back(soccerTextureID_normalMap);
    allTextures.push_back(chairBaseColorTextureID);

    allTextures.push_back(texture_ID_deepslate_emerald_ore);
    allTextures.push_back(texture_ID_deepslate_emerald_ore_n);
    allTextures.push_back(texture_ID_deepslate_emerald_ore_s);
    allTextures.push_back(texture_ID_glass);
    allTextures.push_back(texture_ID_purple_stained_glass);
    allTextures.push_back(testure_ID_iron_block);
    allTextures.push_back(texture_ID_iron_block_n);
    allTextures.push_back(texture_ID_iron_block_s);
    allTextures.push_back(texture_ID_note_block);
    allTextures.push_back(texture_ID_note_block_n);
    allTextures.push_back(texture_ID_note_block_s);
    allTextures.push_back(texture_ID_oak_planks);
    allTextures.push_back(texture_ID_oak_planks_n);
    allTextures.push_back(texture_ID_oak_planks_s);
    allTextures.push_back(texture_ID_stone_bricks);
    allTextures.push_back(texture_ID_stone_bricks_n);
    allTextures.push_back(texture_ID_stone_bricks_s);
    allTextures.push_back(skyboxTextureID);

    allTextures.push_back(earthTextureID);
    allTextures.push_back(earth_nTextureID);
    allTextures.push_back(earth_sTextureID);

    std::cout << "All textures added to the allTextures vector" << std::endl;
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