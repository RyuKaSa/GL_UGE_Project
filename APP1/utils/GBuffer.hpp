// utils/GBuffer.hpp
#ifndef GBUFFER_HPP
#define GBUFFER_HPP

#include <glad/glad.h>

namespace utils {

class GBuffer {
public:
    // Constructor: Initializes the G-buffer with the specified window dimensions
    GBuffer(int width, int height);

    // Destructor: Cleans up allocated OpenGL resources
    ~GBuffer();

    // Bind the G-buffer framebuffer for writing (geometry pass)
    void bindForWriting() const;

    // Getters for the G-buffer textures
    GLuint getPositionTexture() const { return gPosition; }
    GLuint getNormalTexture() const { return gNormal; }
    GLuint getAlbedoSpecTexture() const { return gAlbedoSpec; }

private:
    GLuint gBuffer;       // Framebuffer object
    GLuint gPosition;     // Position texture
    GLuint gNormal;       // Normal texture
    GLuint gAlbedoSpec;   // Albedo + Specular texture
    GLuint rboDepth;      // Renderbuffer for depth
    int m_width;          // Window width
    int m_height;         // Window height
};

} // namespace utils

#endif // GBUFFER_HPP