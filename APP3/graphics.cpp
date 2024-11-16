// graphics.cpp
#include "graphics.hpp"
#include <iostream>
#include <glad/glad.h>

namespace Graphics {

    GBuffer::GBuffer()
        : fbo(0), gPosition(0), gNormal(0), gAlbedoSpec(0), rboDepth(0) {}

    GBuffer::~GBuffer() {
        cleanup();
    }

    bool GBuffer::initialize(int width, int height) {
        // Generate and bind framebuffer
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        // Position color buffer
        glGenTextures(1, &gPosition);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

        // Normal color buffer
        glGenTextures(1, &gNormal);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

        // Albedo + Specular color buffer
        glGenTextures(1, &gAlbedoSpec);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

        // Specify the color attachments for rendering
        GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
        glDrawBuffers(3, attachments);

        // Create and attach depth buffer (renderbuffer)
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

        // Check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "GBuffer Framebuffer not complete!" << std::endl;
            return false;
        }

        // Unbind framebuffer to avoid accidental modifications
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        return true;
    }

    void GBuffer::cleanup() {
        if (fbo != 0) {
            glDeleteFramebuffers(1, &fbo);
            fbo = 0;
        }
        if (gPosition != 0) {
            glDeleteTextures(1, &gPosition);
            gPosition = 0;
        }
        if (gNormal != 0) {
            glDeleteTextures(1, &gNormal);
            gNormal = 0;
        }
        if (gAlbedoSpec != 0) {
            glDeleteTextures(1, &gAlbedoSpec);
            gAlbedoSpec = 0;
        }
        if (rboDepth != 0) {
            glDeleteRenderbuffers(1, &rboDepth);
            rboDepth = 0;
        }
    }

    void deleteTexture(GLuint& textureID) {
        if (textureID != 0) {
            glDeleteTextures(1, &textureID);
            textureID = 0;
            std::cout << "Deleted texture with ID: " << textureID << std::endl;
        }
    }

    void deleteProgram(GLuint& programID) {
        if (programID != 0) {
            glDeleteProgram(programID);
            std::cout << "Deleted shader program with ID: " << programID << std::endl;
            programID = 0;
        }
    }

    void deleteSceneObjectBuffers(SceneObjectManager::SceneObject& obj) {
        if (obj.vaoID != 0) {
            glDeleteVertexArrays(1, &obj.vaoID);
            std::cout << "Deleted VAO with ID: " << obj.vaoID << std::endl;
            obj.vaoID = 0;
        }
    }

} // namespace Graphics