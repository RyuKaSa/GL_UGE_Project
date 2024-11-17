#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat4 uNormalMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

void main() {
    gl_Position = uMVPMatrix * vec4(aPos, 1.0);
    FragPos = vec3(uMVMatrix * vec4(aPos, 1.0));
    Normal = mat3(uNormalMatrix) * aNormal;
    TexCoords = aTexCoords;
}