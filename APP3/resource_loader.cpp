#include "resource_loader.hpp"
#include "texture.hpp"
#include <glimac/Image.hpp>
#include <iostream>
#include <map>

std::map<std::string, GLuint> textures;

// Load room_1 shader
glimac::Program loadRoom_1Shader(const glimac::FilePath& applicationPath) {
    std::string room_1VertexShaderPath = applicationPath.dirPath() + "APP3/shaders/room_1/" + "room_1.vs.glsl";
    std::string room_1FragmentShaderPath = applicationPath.dirPath() + "APP3/shaders/room_1/" + "room_1.fs.glsl";
    
    glimac::Program room_1Program = glimac::loadProgram(room_1VertexShaderPath, room_1FragmentShaderPath);
    if (room_1Program.getGLId() == 0) {
        std::cerr << "Failed to load unified shaders" << std::endl;
    } else {
        std::cout << "room_1 shaders loaded successfully" << std::endl;
    }
    return room_1Program;
}

// Load room_2 shader
glimac::Program loadRoom_2Shader(const glimac::FilePath& applicationPath) {
    std::string room_2VertexShaderPath = applicationPath.dirPath() + "APP3/shaders/room_2/" + "room_2.vs.glsl";
    std::string room_2FragmentShaderPath = applicationPath.dirPath() + "APP3/shaders/room_2/" + "room_2.fs.glsl";
    
    glimac::Program room_2Program = glimac::loadProgram(room_2VertexShaderPath, room_2FragmentShaderPath);
    if (room_2Program.getGLId() == 0) {
        std::cerr << "Failed to load unified shaders" << std::endl;
    } else {
        std::cout << "room_1 shaders loaded successfully" << std::endl;
    }
    return room_2Program;
}

// Load depth shader
glimac::Program loadDepthShader(const glimac::FilePath& applicationPath) {
    std::string depthVertexShaderPath = applicationPath.dirPath() + "APP3/shaders/depth/" + "depth.vs.glsl";
    std::string depthFragmentShaderPath = applicationPath.dirPath() + "APP3/shaders/depth/" + "depth.fs.glsl";

    glimac::Program depthProgram = glimac::loadProgram(depthVertexShaderPath, depthFragmentShaderPath);
    if (depthProgram.getGLId() == 0) {
        std::cerr << "Failed to load depth shaders" << std::endl;
    } else {
        std::cout << "Depth shaders loaded successfully" << std::endl;
    }
    return depthProgram;
}

GLuint loadTexture(const std::string& texturePath) {
    if (textures.find(texturePath) != textures.end()) {
        return textures[texturePath];
    }

    std::unique_ptr<glimac::Image> pImage = glimac::loadImage(texturePath);
    if (!pImage) {
        std::cerr << "Failed to load texture image at " << texturePath << std::endl;
        return 0;
    }

    size_t width = pImage->getWidth();
    size_t height = pImage->getHeight();
    glm::vec4* pixels = pImage->getPixels();

    for (size_t row = 0; row < height / 2; ++row) {
        for (size_t col = 0; col < width; ++col) {
            size_t topIndex = row * width + col;
            size_t bottomIndex = (height - 1 - row) * width + col;
            std::swap(pixels[topIndex], pixels[bottomIndex]);
        }
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, pImage->getWidth(), pImage->getHeight(), 0, GL_RGBA, GL_FLOAT, pImage->getPixels());
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    textures[texturePath] = textureID;
    return textureID;
}

// Load textures
void loadTextures(
    GLuint& brownTerracottaTextureID,
    GLuint& brownTerracottaTextureID_normalMap,
    GLuint& chairBaseColorTextureID,
    GLuint& chairNormalMapTextureID,
    const glimac::FilePath& applicationPath)
{
    brownTerracottaTextureID = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures/brown_glazed_terracotta_8bit.png");
    chairBaseColorTextureID = loadTexture(applicationPath.dirPath() + "../assets/models/Rocking_Chair/Textures/BaseColor.png");

    brownTerracottaTextureID_normalMap = loadTexture(applicationPath.dirPath() + "../APP3/assets/textures/brown_glazed_terracotta_8bit_normal_map.png");
    chairNormalMapTextureID = loadTexture(applicationPath.dirPath() + "../assets/models/Rocking_Chair/Textures/Normal.png");

    std::cout << "Textures loaded" << std::endl;
}