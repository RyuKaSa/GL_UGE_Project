// graphics.hpp
#ifndef GRAPHICS_HPP
#define GRAPHICS_HPP

#include "scene_objects.hpp"
#include <glad/glad.h>

namespace Graphics {

    class GBuffer {
    public:

        GBuffer();
        ~GBuffer();

        GLuint fbo;

        bool initialize(int width, int height);

        void cleanup();

        // Accessor methods
        GLuint getFBO() const { return fbo; }
        GLuint getPositionTexture() const { return gPosition; }
        GLuint getNormalTexture() const { return gNormal; }
        GLuint getAlbedoSpecTexture() const { return gAlbedoSpec; }

    private:
        GLuint gPosition;
        GLuint gNormal;
        GLuint gAlbedoSpec;
        GLuint rboDepth;
    };

    void deleteTexture(GLuint& textureID);

    void deleteProgram(GLuint& programID);

    void deleteSceneObjectBuffers(SceneObjectManager::SceneObject& obj);

} // namespace Graphics

#endif // GRAPHICS_HPP