#version 330 core
layout(location = 0) in vec3 aPosition;
uniform mat4 model;
uniform mat4 shadowMatrix;
out vec4 FragPos;

void main() {
    FragPos = model * vec4(aPosition, 1.0);
    gl_Position = shadowMatrix * FragPos;
}