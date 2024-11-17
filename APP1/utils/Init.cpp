#include "Init.hpp"

namespace utils {

    void createVBOAndVAO(GLuint& vbo, GLuint& vao, const glimac::Sphere& sphere) {
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sphere.getVertexCount() * sizeof(glimac::ShapeVertex), sphere.getDataPointer(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (void*)offsetof(glimac::ShapeVertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (void*)offsetof(glimac::ShapeVertex, normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (void*)offsetof(glimac::ShapeVertex, texCoords));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

} // namespace utils