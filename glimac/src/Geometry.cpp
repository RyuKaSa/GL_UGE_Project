#include "glimac/Geometry.hpp"
#include <iostream>
#include <algorithm>

#define TINYOBJLOADER_IMPLEMENTATION
#include "src/tiny_obj_loader.h"

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

namespace glimac
{

    void Geometry::generateNormals(unsigned int meshIndex) {
        auto& mesh = m_MeshBuffer[meshIndex];
        auto indexOffset = mesh.m_nIndexOffset;
        auto indexCount = mesh.m_nIndexCount;

        // Zero out normals
        for (size_t i = 0; i < m_VertexBuffer.size(); ++i) {
            m_VertexBuffer[i].m_Normal = glm::vec3(0.0f);
        }

        // Compute normals
        for (size_t i = indexOffset; i < indexOffset + indexCount; i += 3) {
            unsigned int i0 = m_IndexBuffer[i];
            unsigned int i1 = m_IndexBuffer[i + 1];
            unsigned int i2 = m_IndexBuffer[i + 2];

            glm::vec3 v0 = m_VertexBuffer[i0].m_Position;
            glm::vec3 v1 = m_VertexBuffer[i1].m_Position;
            glm::vec3 v2 = m_VertexBuffer[i2].m_Position;

            glm::vec3 normal = glm::normalize(glm::cross(v1 - v0, v2 - v0));

            m_VertexBuffer[i0].m_Normal += normal;
            m_VertexBuffer[i1].m_Normal += normal;
            m_VertexBuffer[i2].m_Normal += normal;
        }

        // Normalize the normals
        for (size_t i = 0; i < m_VertexBuffer.size(); ++i) {
            m_VertexBuffer[i].m_Normal = glm::normalize(m_VertexBuffer[i].m_Normal);
        }
    }

    bool Geometry::loadOBJ(const FilePath &filepath, const FilePath &mtlBasePath, bool loadTextures)
    {
        std::clog << "Load OBJ " << filepath << std::endl;

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string err;

        bool ret = tinyobj::LoadObj(
            &attrib,
            &shapes,
            &materials,
            &warn,
            &err,
            filepath.c_str(),
            mtlBasePath.c_str());

        if (!warn.empty())
        {
            std::clog << "TinyOBJLoader Warning: " << warn << std::endl;
        }

        if (!err.empty())
        {
            std::cerr << "TinyOBJLoader Error: " << err << std::endl;
        }

        if (!ret)
        {
            std::cerr << "Failed to load/parse OBJ file: " << filepath << std::endl;
            return false;
        }

        std::clog << "done." << std::endl;

        // Load materials
        std::clog << "Load materials" << std::endl;
        m_Materials.reserve(materials.size());
        for (const auto &material : materials)
        {
            m_Materials.emplace_back();
            auto &m = m_Materials.back();

            m.m_Ka = glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]);
            m.m_Kd = glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
            m.m_Ks = glm::vec3(material.specular[0], material.specular[1], material.specular[2]);
            m.m_Tr = glm::vec3(material.transmittance[0], material.transmittance[1], material.transmittance[2]);
            m.m_Le = glm::vec3(material.emission[0], material.emission[1], material.emission[2]);
            m.m_Shininess = material.shininess;
            m.m_RefractionIndex = material.ior;
            m.m_Dissolve = material.dissolve;

            if (loadTextures)
            {
                if (!material.ambient_texname.empty())
                {
                    FilePath texturePath = mtlBasePath + material.ambient_texname;
                    std::clog << "load " << texturePath << std::endl;
                    m.m_pKaMap = ImageManager::loadImage(texturePath);
                }

                if (!material.diffuse_texname.empty())
                {
                    FilePath texturePath = mtlBasePath + material.diffuse_texname;
                    std::clog << "load " << texturePath << std::endl;
                    m.m_pKdMap = ImageManager::loadImage(texturePath);
                }

                if (!material.specular_texname.empty())
                {
                    FilePath texturePath = mtlBasePath + material.specular_texname;
                    std::clog << "load " << texturePath << std::endl;
                    m.m_pKsMap = ImageManager::loadImage(texturePath);
                }

                if (!material.normal_texname.empty())
                {
                    FilePath texturePath = mtlBasePath + material.normal_texname;
                    std::clog << "load " << texturePath << std::endl;
                    m.m_pNormalMap = ImageManager::loadImage(texturePath);
                }
            }
        }
        std::clog << "done." << std::endl;

        // Prepare to store vertex and index data
        size_t globalVertexOffset = m_VertexBuffer.size();
        size_t globalIndexOffset = m_IndexBuffer.size();

        // Estimate total number of vertices and indices
        size_t nbVertex = 0;
        size_t nbIndex = 0;
        for (const auto &shape : shapes)
        {
            nbIndex += shape.mesh.indices.size();
        }
        nbVertex = attrib.vertices.size() / 3;

        std::clog << "Number of meshes: " << shapes.size() << std::endl;
        std::clog << "Number of vertices: " << nbVertex << std::endl;
        std::clog << "Number of triangles: " << nbIndex / 3 << std::endl;

        // Initialize bounding box
        if (nbVertex > 0)
        {
            m_BBox = BBox3f(glm::vec3(
                attrib.vertices[0],
                attrib.vertices[1],
                attrib.vertices[2]));
        }

        // Resize buffers
        m_VertexBuffer.resize(globalVertexOffset + nbVertex);
        m_IndexBuffer.resize(globalIndexOffset + nbIndex);

        // Create a mapping from tinyobj indices to our vertex buffer indices
        std::unordered_map<tinyobj::index_t, unsigned int, IndexHash, IndexEqual> indexMap;

        // Process shapes
        size_t vertexOffset = globalVertexOffset;
        size_t indexOffset = globalIndexOffset;

        for (size_t s = 0; s < shapes.size(); ++s)
        {
            const tinyobj::shape_t &shape = shapes[s];
            const tinyobj::mesh_t &mesh = shape.mesh;

            size_t meshIndexOffset = indexOffset;
            size_t meshIndexCount = 0;

            // For each face
            size_t faceIndexOffset = 0;
            for (size_t f = 0; f < mesh.num_face_vertices.size(); ++f)
            {
                int fv = mesh.num_face_vertices[f];

                // For each vertex in the face
                for (int v = 0; v < fv; ++v)
                {
                    tinyobj::index_t idx = mesh.indices[faceIndexOffset + v];

                    Vertex vertex;

                    // Positions
                    vertex.m_Position = glm::vec3(
                        attrib.vertices[3 * idx.vertex_index + 0],
                        attrib.vertices[3 * idx.vertex_index + 1],
                        attrib.vertices[3 * idx.vertex_index + 2]);

                    // Normals
                    if (idx.normal_index >= 0)
                    {
                        vertex.m_Normal = glm::vec3(
                            attrib.normals[3 * idx.normal_index + 0],
                            attrib.normals[3 * idx.normal_index + 1],
                            attrib.normals[3 * idx.normal_index + 2]);
                    }
                    else
                    {
                        vertex.m_Normal = glm::vec3(0.0f);
                    }

                    // Texture coordinates
                    if (idx.texcoord_index >= 0)
                    {
                        vertex.m_TexCoords = glm::vec2(
                            attrib.texcoords[2 * idx.texcoord_index + 0],
                            attrib.texcoords[2 * idx.texcoord_index + 1]);
                    }
                    else
                    {
                        vertex.m_TexCoords = glm::vec2(0.0f);
                    }

                    // Update bounding box
                    m_BBox.grow(vertex.m_Position);

                    // Check if the vertex is already in the buffer
                    if (indexMap.count(idx) == 0)
                    {
                        m_VertexBuffer[vertexOffset] = vertex;
                        indexMap[idx] = static_cast<unsigned int>(vertexOffset);
                        vertexOffset++;
                    }

                    // Add the index
                    m_IndexBuffer[indexOffset] = indexMap[idx];
                    indexOffset++;
                    meshIndexCount++;
                }
                faceIndexOffset += fv;
            }

            // Get material index
            int materialIndex = -1;
            if (!mesh.material_ids.empty())
            {
                materialIndex = mesh.material_ids[0];
            }

            // Create Mesh entry
            m_MeshBuffer.emplace_back(
                shape.name,
                static_cast<unsigned int>(meshIndexOffset),
                static_cast<unsigned int>(meshIndexCount),
                materialIndex);

            // Generate normals if they are missing
            if (attrib.normals.empty())
            {
                generateNormals(m_MeshBuffer.size() - 1);
            }
        }

        return true;
    }
}
