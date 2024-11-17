// utils/Renderer.hpp
#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.hpp"
#include "GBuffer.hpp"
#include "Camera.hpp"
#include "Lighting.hpp"
#include <vector>

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
    /**
     * @brief Constructs the Renderer with specified window dimensions.
     * 
     * @param width The width of the window.
     * @param height The height of the window.
     */
    Renderer(int width, int height);

    /**
     * @brief Destructor.
     */
    ~Renderer();

    /**
     * @brief Performs the geometry pass, rendering scene geometry to the G-buffer.
     * 
     * @param camera The Camera object representing the current view.
     * @param vao The Vertex Array Object of the geometry to render.
     * @param modelMatrix The model transformation matrix.
     * @param gBufferShader The Shader used for the geometry pass.
     * @param vertexCount The number of vertices to draw.
     */
    void geometryPass(const Camera& camera, GLuint vao, const glm::mat4& modelMatrix, Shader& gBufferShader, GLsizei vertexCount, int width, int height);

    /**
     * @brief Performs the lighting pass, computing lighting using the G-buffer.
     * 
     * @param camera The Camera object representing the current view.
     * @param quadVAO The Vertex Array Object of the full-screen quad.
     * @param pointLights A vector of PointLight structs representing the scene's point lights.
     * @param lightingShader The Shader used for the lighting pass.
     */
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