#include "texture.hpp"
#include <iostream>
#include <glimac/Image.hpp>
#include <glm/glm.hpp>

std::map<std::string, GLuint> textures;

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