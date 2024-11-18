// utils/Renderer.cpp
#include "Renderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace utils {

Renderer::Renderer(int width, int height) : m_gBuffer(width, height) {}

Renderer::~Renderer() {}

void Renderer::geometryPass(const Camera& camera, GLuint sphereVAO, const std::vector<objects::Sphere>& spheres, Shader& gBufferShader, GLsizei vertexCount, int width, int height) {
    m_gBuffer.bindForWriting();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    gBufferShader.use();

    glm::mat4 ProjMatrix = glm::perspective(glm::radians(70.0f), static_cast<float>(width) / height, 0.1f, 100.0f);
    glm::mat4 ViewMatrix = camera.getViewMatrix();

    for (const auto& sphere : spheres) {
        glm::mat4 ModelMatrix = glm::translate(glm::mat4(1.0f), sphere.position) * glm::scale(glm::mat4(1.0f), glm::vec3(sphere.size));
        glm::mat4 MVPMatrix = ProjMatrix * ViewMatrix * ModelMatrix;
        glm::mat4 NormalMatrix = glm::transpose(glm::inverse(ModelMatrix));

        GLint uMVPMatrixLocation = gBufferShader.getUniformLocation("uMVPMatrix");
        GLint uNormalMatrixLocation = gBufferShader.getUniformLocation("uNormalMatrix");
        GLint uModelMatrixLocation = gBufferShader.getUniformLocation("uModelMatrix");
        GLint uColorLocation = gBufferShader.getUniformLocation("uColor");

        if (uMVPMatrixLocation != -1) glUniformMatrix4fv(uMVPMatrixLocation, 1, GL_FALSE, glm::value_ptr(MVPMatrix));
        if (uNormalMatrixLocation != -1) glUniformMatrix4fv(uNormalMatrixLocation, 1, GL_FALSE, glm::value_ptr(NormalMatrix));
        if (uModelMatrixLocation != -1) glUniformMatrix4fv(uModelMatrixLocation, 1, GL_FALSE, glm::value_ptr(ModelMatrix));
        if (uColorLocation != -1) glUniform3fv(uColorLocation, 1, glm::value_ptr(sphere.color));

        glBindVertexArray(sphereVAO);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        glBindVertexArray(0);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::lightingPass(const Camera& camera, GLuint quadVAO, const std::vector<PointLight>& pointLights, Shader& lightingShader) {
    // Clear buffers for lighting pass
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the lighting shader
    lightingShader.use();

    // Set Blinn-Phong uniforms
    GLint uKsLocation = lightingShader.getUniformLocation("uKs");
    GLint uShininessLocation = lightingShader.getUniformLocation("uShininess");
    GLint uCameraPosLocation = lightingShader.getUniformLocation("cameraPosWorld");

    if (uKsLocation != -1) {
        glUniform3f(uKsLocation, 0.5f, 0.5f, 0.5f); // Specular reflection coefficient
    } else {
        std::cerr << "Renderer::lightingPass: uKs not found!" << std::endl;
    }

    if (uShininessLocation != -1) {
        glUniform1f(uShininessLocation, 32.0f); // Shininess exponent
    } else {
        std::cerr << "Renderer::lightingPass: uShininess not found!" << std::endl;
    }

    if (uCameraPosLocation != -1) {
        glm::vec3 cameraPos = camera.getPosition();
        glUniform3fv(uCameraPosLocation, 1, glm::value_ptr(cameraPos));
    } else {
        std::cerr << "Renderer::lightingPass: cameraPosWorld not found!" << std::endl;
    }

    // Bind G-buffer textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getPositionTexture());
    glUniform1i(lightingShader.getUniformLocation("gPosition"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getNormalTexture());
    glUniform1i(lightingShader.getUniformLocation("gNormal"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_gBuffer.getAlbedoSpecTexture());
    glUniform1i(lightingShader.getUniformLocation("gAlbedoSpec"), 2);

    // Set number of point lights
    GLint numPointLightsLoc = lightingShader.getUniformLocation("numPointLights");
    if (numPointLightsLoc != -1) {
        glUniform1i(numPointLightsLoc, static_cast<int>(pointLights.size()));
    } else {
        std::cerr << "Renderer::lightingPass: numPointLights not found!" << std::endl;
    }

    // Set point lights uniforms
    for (size_t i = 0; i < pointLights.size(); ++i) {
        std::string baseName = "pointLights[" + std::to_string(i) + "]";
        std::string positionName = baseName + ".position";
        std::string colorName = baseName + ".color";
        std::string intensityName = baseName + ".intensity";

        GLint positionLoc = lightingShader.getUniformLocation(positionName);
        GLint colorLoc = lightingShader.getUniformLocation(colorName);
        GLint intensityLoc = lightingShader.getUniformLocation(intensityName);

        if (positionLoc != -1 && colorLoc != -1 && intensityLoc != -1) {
            glUniform3fv(positionLoc, 1, glm::value_ptr(pointLights[i].position));
            glUniform3fv(colorLoc, 1, glm::value_ptr(pointLights[i].color));
            glUniform1f(intensityLoc, pointLights[i].intensity);
        } else {
            std::cerr << "Renderer::lightingPass: Failed to get uniform locations for pointLights[" << i << "]" << std::endl;
        }
    }

    // Render the full-screen quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

} // namespace utils