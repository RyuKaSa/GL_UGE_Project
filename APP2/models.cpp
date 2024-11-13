#include "models.hpp"
#include <src/tiny_obj_loader.h>
#include <src/stb_image.h>
#include <iostream>

GLuint LoadTextureFromFile(const char* path) {
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture parameters (wrapping, filtering)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);

        return textureID;
    } else {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
        return 0;
    }
}

bool loadOBJ(const std::string& filePath, const std::string& basePath, ModelData &modelData) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // **Updated TinyOBJLoader function call**
    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str(), basePath.c_str());

    if (!warn.empty()) {
        std::cerr << "TinyOBJLoader Warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "TinyOBJLoader Error: " << err << std::endl;
        return false;
    }

    if (!ret) {
        std::cerr << "Failed to load/parse OBJ file: " << filePath << std::endl;
        return false;
    }

    // Initialize vectors
    modelData.vertices.clear();
    modelData.normals.clear();
    modelData.texcoords.clear();
    modelData.indices.clear();

    // Map to store unique vertices
    // **Use unordered_map with custom hash and equality functions**
    struct IndexHash {
        size_t operator()(const tinyobj::index_t& idx) const {
            return std::hash<int>()(idx.vertex_index) ^
                   std::hash<int>()(idx.normal_index) ^
                   std::hash<int>()(idx.texcoord_index);
        }
    };

    struct IndexEqual {
        bool operator()(const tinyobj::index_t& lhs, const tinyobj::index_t& rhs) const {
            return lhs.vertex_index == rhs.vertex_index &&
                   lhs.normal_index == rhs.normal_index &&
                   lhs.texcoord_index == rhs.texcoord_index;
        }
    };

    std::unordered_map<tinyobj::index_t, unsigned int, IndexHash, IndexEqual> uniqueVertices;

    // Loop over shapes
    for (size_t s = 0; s < shapes.size(); ++s) {
        const tinyobj::mesh_t& mesh = shapes[s].mesh;

        size_t indexOffset = 0;
        for (size_t f = 0; f < mesh.num_face_vertices.size(); ++f) {
            int fv = mesh.num_face_vertices[f];

            // For each vertex in the face
            for (size_t v = 0; v < static_cast<size_t>(fv); ++v) {
                tinyobj::index_t idx = mesh.indices[indexOffset + v];

                // **Process vertex using attrib arrays**
                // Positions
                glm::vec3 position(
                    attrib.vertices[3 * idx.vertex_index + 0],
                    attrib.vertices[3 * idx.vertex_index + 1],
                    attrib.vertices[3 * idx.vertex_index + 2]
                );

                // Normals
                glm::vec3 normal(0.0f, 0.0f, 0.0f);
                if (idx.normal_index >= 0) {
                    normal = glm::vec3(
                        attrib.normals[3 * idx.normal_index + 0],
                        attrib.normals[3 * idx.normal_index + 1],
                        attrib.normals[3 * idx.normal_index + 2]
                    );
                }

                // Texture coordinates
                glm::vec2 texcoord(0.0f, 0.0f);
                if (idx.texcoord_index >= 0) {
                    texcoord = glm::vec2(
                        attrib.texcoords[2 * idx.texcoord_index + 0],
                        attrib.texcoords[2 * idx.texcoord_index + 1]
                    );
                }

                // Avoid duplicate vertices
                if (uniqueVertices.count(idx) == 0) {
                    uniqueVertices[idx] = static_cast<unsigned int>(modelData.vertices.size() / 3);

                    // Add vertex data
                    modelData.vertices.push_back(position.x);
                    modelData.vertices.push_back(position.y);
                    modelData.vertices.push_back(position.z);

                    modelData.normals.push_back(normal.x);
                    modelData.normals.push_back(normal.y);
                    modelData.normals.push_back(normal.z);

                    modelData.texcoords.push_back(texcoord.x);
                    modelData.texcoords.push_back(texcoord.y);
                }

                modelData.indices.push_back(uniqueVertices[idx]);
            }
            indexOffset += fv;
        }
    }

    // If normals are empty or zero, compute them
    if (modelData.normals.empty() || modelData.normals[0] == 0.0f) {
        // Compute normals
        size_t numFaces = modelData.indices.size() / 3;
        for (size_t i = 0; i < numFaces; ++i) {
            unsigned int idx0 = modelData.indices[3 * i + 0];
            unsigned int idx1 = modelData.indices[3 * i + 1];
            unsigned int idx2 = modelData.indices[3 * i + 2];

            glm::vec3 v0(modelData.vertices[3 * idx0 + 0], modelData.vertices[3 * idx0 + 1], modelData.vertices[3 * idx0 + 2]);
            glm::vec3 v1(modelData.vertices[3 * idx1 + 0], modelData.vertices[3 * idx1 + 1], modelData.vertices[3 * idx1 + 2]);
            glm::vec3 v2(modelData.vertices[3 * idx2 + 0], modelData.vertices[3 * idx2 + 1], modelData.vertices[3 * idx2 + 2]);

            glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

            for (int j = 0; j < 3; ++j) {
                modelData.normals[3 * modelData.indices[3 * i + j] + 0] += normal.x;
                modelData.normals[3 * modelData.indices[3 * i + j] + 1] += normal.y;
                modelData.normals[3 * modelData.indices[3 * i + j] + 2] += normal.z;
            }
        }

        // Normalize the normals
        size_t numVertices = modelData.vertices.size() / 3;
        for (size_t i = 0; i < numVertices; ++i) {
            glm::vec3 normal(
                modelData.normals[3 * i + 0],
                modelData.normals[3 * i + 1],
                modelData.normals[3 * i + 2]
            );
            normal = glm::normalize(normal);
            modelData.normals[3 * i + 0] = normal.x;
            modelData.normals[3 * i + 1] = normal.y;
            modelData.normals[3 * i + 2] = normal.z;
        }
    }

    // Load textures for materials
    for (size_t i = 0; i < materials.size(); ++i) {
        tinyobj::material_t& mat = materials[i];
        if (!mat.diffuse_texname.empty()) {
            std::string texturePath = basePath + mat.diffuse_texname;
            GLuint textureID = LoadTextureFromFile(texturePath.c_str());
            modelData.materialToTexture[i] = textureID;
        } else {
            modelData.materialToTexture[i] = 0; // No texture
        }
    }

    // Store materials and shapes
    modelData.materials = materials;
    modelData.shapes = shapes;

    return true;
}

void setupModelBuffers(ModelData &modelData) {
    glGenVertexArrays(1, &modelData.vao);
    glGenBuffers(1, &modelData.vbo);
    glGenBuffers(1, &modelData.ebo);

    glBindVertexArray(modelData.vao);

    // Interleave data
    std::vector<float> interleavedData;
    size_t numVertices = modelData.vertices.size() / 3; // Assuming positions are x,y,z

    for (size_t i = 0; i < numVertices; ++i) {
        // Positions (x, y, z)
        interleavedData.push_back(modelData.vertices[3 * i]);
        interleavedData.push_back(modelData.vertices[3 * i + 1]);
        interleavedData.push_back(modelData.vertices[3 * i + 2]);

        // Normals (nx, ny, nz)
        interleavedData.push_back(modelData.normals[3 * i]);
        interleavedData.push_back(modelData.normals[3 * i + 1]);
        interleavedData.push_back(modelData.normals[3 * i + 2]);

        // Texture Coordinates (u, v)
        interleavedData.push_back(modelData.texcoords[2 * i]);
        interleavedData.push_back(modelData.texcoords[2 * i + 1]);
    }

    // Upload interleaved data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, modelData.vbo);
    glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float), interleavedData.data(), GL_STATIC_DRAW);

    // Upload index data to EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelData.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelData.indices.size() * sizeof(unsigned int), modelData.indices.data(), GL_STATIC_DRAW);

    // Set up vertex attributes
    GLsizei stride = (3 + 3 + 2) * sizeof(float); // Position (3) + Normal (3) + Texcoord (2)
    size_t offset = 0;

    // Position attribute
    glEnableVertexAttribArray(0); // Assuming location 0 in shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    offset += 3 * sizeof(float);

    // Normal attribute
    glEnableVertexAttribArray(1); // Assuming location 1 in shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    offset += 3 * sizeof(float);

    // Texture coordinate attribute
    glEnableVertexAttribArray(2); // Assuming location 2 in shader
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);

    glBindVertexArray(0);
}