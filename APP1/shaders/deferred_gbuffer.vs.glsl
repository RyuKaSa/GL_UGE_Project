#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 uMVPMatrix;
uniform mat4 uNormalMatrix;
uniform mat4 uModelMatrix; // Ensure this is correctly set

out vec3 FragPosWorld;
out vec3 NormalWorld;
out vec2 TexCoords;

void main() {
    gl_Position = uMVPMatrix * vec4(aPos, 1.0);
    FragPosWorld = vec3(uModelMatrix * vec4(aPos, 1.0)); // Position in world space
    NormalWorld = mat3(uNormalMatrix) * aNormal; // Normal in world space
    TexCoords = aTexCoords;
}