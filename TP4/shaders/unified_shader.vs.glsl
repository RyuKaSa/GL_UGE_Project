#version 330 core

layout(location = 0) in vec3 aPosition;     // Vertex position
layout(location = 1) in vec3 aNormal;       // Vertex normal
layout(location = 2) in vec2 aTexCoords;    // Texture coordinates

uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNormalMatrix;

out vec3 vNormal;
out vec3 vFragPos;
out vec2 vTexCoords;

void main()
{
    vNormal = mat3(uNormalMatrix) * aNormal;
    vFragPos = vec3(uMVMatrix * vec4(aPosition, 1.0));
    vTexCoords = aTexCoords;
    gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
}