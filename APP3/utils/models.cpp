#include "models.hpp"
#include <src/tiny_obj_loader.h>
#include <src/stb_image.h>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp> 
#include <cmath> 

namespace utils_object {

std::vector<GLuint> allTextures;

void GetRockingChairPositionAndRotation(
    double currentTime,
    double frequency,
    double radius,
    double length,
    glm::vec3& position,
    glm::vec3& rotation)
{
    // Calculate angular frequency and time-dependent angle for rolling motion
    double omega = 2.0 * glm::pi<double>() * frequency; // Angular frequency
    double displacement = length * 0.5 * sin(omega * currentTime); // Point P's back and forth motion, clamped to [-length/2, length/2]

    // Compute the angle of rotation based on the displacement of point P
    double theta = displacement / radius; // Rotation angle in radians, derived from s = r * theta

    // Set position (displacement along the X-axis, assuming rolling along X-axis)
    position = glm::vec3(0.0, 0.0, -displacement); // Adjust axes as necessary for your specific setup

    // Set rotation around the Z-axis to simulate rolling (assuming circle rolls along X-axis)
    rotation = glm::vec3(0.0, 0.0, -1.0 * static_cast<float>(glm::degrees(theta))); // Convert theta to degrees for rotation
}

GLuint LoadTextureFromFile(const char* path) {
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format = GL_RGB;
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

        // print ID
        std::cout << "Texture ID: " << textureID << std::endl;

        // register the texture, easier for cleanup
        allTextures.push_back(textureID);

        return textureID;
    } else {
        std::cerr << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
        return 0;
    }
}

void computeTangents(ModelData &model) {
    size_t vertexCount = model.vertices.size() / 3;
    size_t indexCount = model.indices.size();

    // Initialize tangents and bitangents
    model.tangents.assign(vertexCount * 3, 0.0f);
    model.bitangents.assign(vertexCount * 3, 0.0f);

    // Iterate over each triangle
    for (size_t i = 0; i < indexCount; i += 3) {
        unsigned int index0 = model.indices[i];
        unsigned int index1 = model.indices[i + 1];
        unsigned int index2 = model.indices[i + 2];

        // Positions
        glm::vec3 pos0(model.vertices[index0 * 3 + 0], model.vertices[index0 * 3 + 1], model.vertices[index0 * 3 + 2]);
        glm::vec3 pos1(model.vertices[index1 * 3 + 0], model.vertices[index1 * 3 + 1], model.vertices[index1 * 3 + 2]);
        glm::vec3 pos2(model.vertices[index2 * 3 + 0], model.vertices[index2 * 3 + 1], model.vertices[index2 * 3 + 2]);

        // Texture coordinates
        glm::vec2 uv0(model.texcoords[index0 * 2 + 0], model.texcoords[index0 * 2 + 1]);
        glm::vec2 uv1(model.texcoords[index1 * 2 + 0], model.texcoords[index1 * 2 + 1]);
        glm::vec2 uv2(model.texcoords[index2 * 2 + 0], model.texcoords[index2 * 2 + 1]);

        // Edges of the triangle
        glm::vec3 edge1 = pos1 - pos0;
        glm::vec3 edge2 = pos2 - pos0;

        glm::vec2 deltaUV1 = uv1 - uv0;
        glm::vec2 deltaUV2 = uv2 - uv0;

        float f = 1.0f;
        float denominator = (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
        if (denominator != 0.0f) {
            f = 1.0f / denominator;
        }

        glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
        glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

        // Accumulate the tangents and bitangents
        for (int j = 0; j < 3; ++j) {
            unsigned int idx = model.indices[i + j];
            model.tangents[idx * 3 + 0] += tangent.x;
            model.tangents[idx * 3 + 1] += tangent.y;
            model.tangents[idx * 3 + 2] += tangent.z;

            model.bitangents[idx * 3 + 0] += bitangent.x;
            model.bitangents[idx * 3 + 1] += bitangent.y;
            model.bitangents[idx * 3 + 2] += bitangent.z;
        }
    }

    // Normalize the tangents and bitangents
    for (size_t i = 0; i < vertexCount; ++i) {
        glm::vec3 tangent(model.tangents[i * 3 + 0], model.tangents[i * 3 + 1], model.tangents[i * 3 + 2]);
        tangent = glm::normalize(tangent);
        model.tangents[i * 3 + 0] = tangent.x;
        model.tangents[i * 3 + 1] = tangent.y;
        model.tangents[i * 3 + 2] = tangent.z;

        glm::vec3 bitangent(model.bitangents[i * 3 + 0], model.bitangents[i * 3 + 1], model.bitangents[i * 3 + 2]);
        bitangent = glm::normalize(bitangent);
        model.bitangents[i * 3 + 0] = bitangent.x;
        model.bitangents[i * 3 + 1] = bitangent.y;
        model.bitangents[i * 3 + 2] = bitangent.z;
    }
}

void centerModel(ModelData& modelData) {
    // Compute the centroid of the model
    glm::vec3 centroid(0.0f);
    size_t vertexCount = modelData.vertices.size() / 3;

    for (size_t i = 0; i < vertexCount; ++i) {
        centroid.x += modelData.vertices[3 * i + 0];
        centroid.y += modelData.vertices[3 * i + 1];
        centroid.z += modelData.vertices[3 * i + 2];
    }
    centroid /= static_cast<float>(vertexCount);

    // Find the minimum Z (height) value among all vertices
    float minZ = std::numeric_limits<float>::max();
    for (size_t i = 0; i < vertexCount; ++i) {
        minZ = std::min(minZ, modelData.vertices[3 * i + 2]);
    }

    // Calculate the height adjustment needed to move the centroid to the minimum Z height
    float heightAdjustment = centroid.z - minZ;

    // Translate vertices to move the centroid to (centroid.x, centroid.y, minZ)
    for (size_t i = 0; i < vertexCount; ++i) {
        modelData.vertices[3 * i + 0] -= centroid.x; // Center X
        modelData.vertices[3 * i + 1] -= centroid.y; // Center Y
        modelData.vertices[3 * i + 2] -= heightAdjustment; // Move Z to minZ
    }
}

bool loadOBJ(const std::string& filePath, const std::string& basePath, ModelData &modelData) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    // Load OBJ file
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
    modelData.tangents.clear();
    modelData.bitangents.clear();

    // Map to store unique vertices
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

                // Process vertex using attrib arrays
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

                    // Initialize tangents and bitangents
                    modelData.tangents.push_back(0.0f);
                    modelData.tangents.push_back(0.0f);
                    modelData.tangents.push_back(0.0f);

                    modelData.bitangents.push_back(0.0f);
                    modelData.bitangents.push_back(0.0f);
                    modelData.bitangents.push_back(0.0f);
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

    // Compute tangents and bitangents
    computeTangents(modelData);

    // centerModel(modelData);

    return true;
}

void setupModelBuffers(ModelData &modelData) {
    glGenVertexArrays(1, &modelData.vao);
    glGenBuffers(1, &modelData.vbo);
    glGenBuffers(1, &modelData.ebo);

    glBindVertexArray(modelData.vao);

    // Interleave data: Position (3) + Normal (3) + Texcoord (2) + Tangent (3) + Bitangent (3)
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

        // Tangents (tx, ty, tz)
        interleavedData.push_back(modelData.tangents[3 * i]);
        interleavedData.push_back(modelData.tangents[3 * i + 1]);
        interleavedData.push_back(modelData.tangents[3 * i + 2]);

        // Bitangents (bx, by, bz)
        interleavedData.push_back(modelData.bitangents[3 * i]);
        interleavedData.push_back(modelData.bitangents[3 * i + 1]);
        interleavedData.push_back(modelData.bitangents[3 * i + 2]);
    }

    // Upload interleaved data to VBO
    glBindBuffer(GL_ARRAY_BUFFER, modelData.vbo);
    glBufferData(GL_ARRAY_BUFFER, interleavedData.size() * sizeof(float), interleavedData.data(), GL_STATIC_DRAW);

    // Upload index data to EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, modelData.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelData.indices.size() * sizeof(unsigned int), modelData.indices.data(), GL_STATIC_DRAW);

    // Set up vertex attributes
    GLsizei stride = (3 + 3 + 2 + 3 + 3) * sizeof(float); // Position, Normal, Texcoord, Tangent, Bitangent
    size_t offset = 0;

    // Position attribute (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    offset += 3 * sizeof(float);

    // Normal attribute (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    offset += 3 * sizeof(float);

    // Texture Coordinate attribute (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    offset += 2 * sizeof(float);

    // Tangent attribute (location = 3)
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    offset += 3 * sizeof(float);

    // Bitangent attribute (location = 4)
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, stride, (void*)offset);
    offset += 3 * sizeof(float);

    glBindVertexArray(0);
}

} // namespace utils_scene