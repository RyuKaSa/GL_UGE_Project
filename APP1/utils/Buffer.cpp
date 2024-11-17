// utils/Buffer.cpp
#include "Buffer.hpp"
#include "Renderer.hpp"
#include <cstddef> // For offsetof
#include <iostream>

namespace utils {

void createVBOAndVAO(GLuint& vbo, GLuint& vao, const glimac::Sphere& sphere) {
    // Generate and bind the VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sphere.getVertexCount() * sizeof(glimac::ShapeVertex), sphere.getDataPointer(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Generate and bind the VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Enable and set vertex attribute pointers
    glEnableVertexAttribArray(0); // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (void*)offsetof(glimac::ShapeVertex, position));

    glEnableVertexAttribArray(1); // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (void*)offsetof(glimac::ShapeVertex, normal));

    glEnableVertexAttribArray(2); // Texture Coordinates attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (void*)offsetof(glimac::ShapeVertex, texCoords));

    // Unbind the VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

GLuint createFullScreenQuad() {
    GLuint quadVAO, quadVBO;
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f,  1.0f,
        -1.0f, -1.0f,  0.0f,  0.0f,
         1.0f, -1.0f,  1.0f,  0.0f,

        -1.0f,  1.0f,  0.0f,  1.0f,
         1.0f, -1.0f,  1.0f,  0.0f,
         1.0f,  1.0f,  1.0f,  1.0f
    };
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    glBindVertexArray(quadVAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    
    // Texture Coordinates attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    glBindVertexArray(0);
    
    // Optionally, you can delete the VBO if you don't need to modify it later
    glDeleteBuffers(1, &quadVBO);
    
    return quadVAO;
}

} // namespace utils