#version 330 core

layout(location = 0) in vec3 aPosition;   // Vertex position
layout(location = 1) in vec3 aNormal;     // Vertex normal
layout(location = 2) in vec2 aTexCoords;  // Texture coordinates (if any)

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform mat4 uMVPMatrix;
uniform mat4 uNormalMatrix;

uniform vec3 uColor; // Sphere color

out vec3 FragPos;    // Position in world space
out vec3 Normal;     // Normal in world space
out vec3 Albedo;     // Albedo color

void main() {
    gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
    FragPos = vec3(uModelMatrix * vec4(aPosition, 1.0));
    Normal = mat3(uNormalMatrix) * aNormal;
    Albedo = uColor; // Pass color to the fragment shader
}