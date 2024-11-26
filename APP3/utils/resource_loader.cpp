#include "resource_loader.hpp"
#include "texture.hpp"
#include <glimac/Image.hpp>
#include <iostream>

namespace utils_loader {

// Load textures
void loadTextures(GLuint& textureID, GLuint& stoneTextureID, GLuint& brownTerracottaTextureID, GLuint& soccerTextureID,
                  GLuint& textureID_normalMap, GLuint& stoneTextureID_normalMap, GLuint& brownTerracottaTextureID_normalMap, GLuint& soccerTextureID_normalMap,
                  GLuint& chairBaseColorTextureID, GLuint& chairNormalMapTextureID,
                  const glimac::FilePath& applicationPath) {
    textureID = loadTexture(applicationPath.dirPath() + "../APP1/assets/textures_HD/cobblestone_8bit.png");
    stoneTextureID = loadTexture(applicationPath.dirPath() + "../APP1/assets/textures_HD/stone_8bit.png");
    brownTerracottaTextureID = loadTexture(applicationPath.dirPath() + "../APP1/assets/textures_HD/brown_glazed_terracotta_8bit.png");
    soccerTextureID = loadTexture(applicationPath.dirPath() + "../APP1/assets/textures_sphere/soccer_sph_s_8bit.png");
    chairBaseColorTextureID = loadTexture(applicationPath.dirPath() + "../assets/models/Rocking_Chair/Textures/BaseColor.png");

    textureID_normalMap = loadTexture(applicationPath.dirPath() + "../APP1/assets/textures_HD/cobblestone_8bit_normal_map.png");
    stoneTextureID_normalMap = loadTexture(applicationPath.dirPath() + "../APP1/assets/textures_HD/stone_8bit_normal_map.png");
    brownTerracottaTextureID_normalMap = loadTexture(applicationPath.dirPath() + "../APP1/assets/textures_HD/brown_glazed_terracotta_8bit_normal_map.png");
    soccerTextureID_normalMap = loadTexture(applicationPath.dirPath() + "../APP1/assets/textures_sphere/soccer_sph_s_8bit_normal_map.png");
    chairNormalMapTextureID = loadTexture(applicationPath.dirPath() + "../assets/models/Rocking_Chair/Textures/Normal.png");

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