#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include <map>
#include <glad/glad.h>

extern std::map<std::string, GLuint> textures;

GLuint loadTexture(const std::string& texturePath);

#endif // TEXTURE_HPP