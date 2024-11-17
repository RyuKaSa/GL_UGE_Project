#include "Render.hpp"

namespace utils {

    void renderSphere(GLuint vao, const glimac::Sphere& sphere) {
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, sphere.getVertexCount());
        glBindVertexArray(0);
    }

} // namespace utils