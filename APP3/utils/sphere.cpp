#include "sphere.hpp"
#include <glm/glm.hpp>

size_t createSphereVertices(std::vector<SphereVertex>& sphereVertices, glimac::Sphere& sphere) {
    size_t sphereVertexCount = sphere.getVertexCount();
    const glimac::ShapeVertex* sphereData = sphere.getDataPointer();
    sphereVertices.resize(sphereVertexCount);

    for (size_t i = 0; i < sphereVertexCount; ++i) {
        sphereVertices[i].position = sphereData[i].position;
        sphereVertices[i].normal = sphereData[i].normal;
        sphereVertices[i].texCoords = sphereData[i].texCoords;
        sphereVertices[i].tangent = glm::vec3(0.0f);
        sphereVertices[i].bitangent = glm::vec3(0.0f);
    }

    return sphereVertexCount; // Return the vertex count
}

void computeSphereTangents(std::vector<SphereVertex>& vertices) {
    for (auto& vertex : vertices) {
        vertex.tangent = glm::vec3(0.0f);
        vertex.bitangent = glm::vec3(0.0f);
    }

    for (size_t i = 0; i < vertices.size(); i += 3) {
        SphereVertex& v0 = vertices[i];
        SphereVertex& v1 = vertices[i + 1];
        SphereVertex& v2 = vertices[i + 2];

        glm::vec3 edge1 = v1.position - v0.position;
        glm::vec3 edge2 = v2.position - v0.position;

        glm::vec2 deltaUV1 = v1.texCoords - v0.texCoords;
        glm::vec2 deltaUV2 = v2.texCoords - v0.texCoords;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
        glm::vec3 bitangent = f * (-deltaUV2.x * edge1 + deltaUV1.x * edge2);

        v0.tangent += tangent;
        v1.tangent += tangent;
        v2.tangent += tangent;

        v0.bitangent += bitangent;
        v1.bitangent += bitangent;
        v2.bitangent += bitangent;
    }

    for (auto& vertex : vertices) {
        vertex.tangent = glm::normalize(vertex.tangent);
        vertex.bitangent = glm::normalize(vertex.bitangent);
    }
}

void setupSphereBuffers(const std::vector<SphereVertex>& sphereVertices, GLuint& sphereVBO, GLuint& sphereVAO) {
    glGenBuffers(1, &sphereVBO);
    glGenVertexArrays(1, &sphereVAO);

    glBindVertexArray(sphereVAO);

    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(SphereVertex), sphereVertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, texCoords));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(SphereVertex), (void*)offsetof(SphereVertex, bitangent));

    glBindVertexArray(0);
}