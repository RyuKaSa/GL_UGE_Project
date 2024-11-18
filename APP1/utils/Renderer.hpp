// utils/Renderer.hpp
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "Shader.hpp"
#include "GBuffer.hpp"
#include "Camera.hpp"
#include "Lighting.hpp"
#include "Objects.hpp"


struct ShapeVertex {
    glm::vec3 position;   // Position of the vertex
    glm::vec3 normal;     // Normal vector at the vertex
    glm::vec2 texCoords;  // Texture coordinates
};

namespace utils {

/**
 * @brief Encapsulates the rendering process, handling both geometry and lighting passes.
 */
class Renderer {
public:
    Renderer(int width, int height);

    ~Renderer();

    void geometryPass(const Camera& camera, GLuint sphereVAO, const std::vector<objects::Sphere>& spheres, Shader& gBufferShader, GLsizei vertexCount, int width, int height);
    
    void lightingPass(const Camera& camera, GLuint quadVAO, const std::vector<PointLight>& pointLights, Shader& lightingShader);

    // Getters for G-buffer textures
    GLuint getGPosition() const { return m_gBuffer.getPositionTexture(); }
    GLuint getGNormal() const { return m_gBuffer.getNormalTexture(); }
    GLuint getGAlbedoSpec() const { return m_gBuffer.getAlbedoSpecTexture(); }

private:
    GBuffer m_gBuffer; // Encapsulates the G-buffer
};

} // namespace utils

#endif // RENDERER_HPP