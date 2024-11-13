#ifndef MODELS_HPP
#define MODELS_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include "global.hpp"
#include <src/tiny_obj_loader.h>

struct ModelData {
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texcoords;
    std::vector<unsigned int> indices;
    std::vector<tinyobj::material_t> materials;
    std::vector<tinyobj::shape_t> shapes;
    std::map<int, GLuint> materialToTexture; // Map material ID to texture ID
    GLuint vao, vbo, ebo;
    std::vector<float> tangents;
    std::vector<float> bitangents;
};

// Function to load a texture from file and return its OpenGL texture ID
GLuint LoadTextureFromFile(const char* path);

void computeTangents(ModelData &modelData);

// Function to load an OBJ model using TinyOBJLoader
bool loadOBJ(const std::string& filePath, const std::string& basePath, ModelData &modelData);

// Function to set up OpenGL buffers (VAO, VBO, EBO) for the model
void setupModelBuffers(ModelData &modelData);

#endif // MODELS_HPP