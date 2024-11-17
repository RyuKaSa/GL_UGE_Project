#version 330 core

layout(location = 0) in vec2 aPos; // Changed from vec3 to vec2

void main() {
    gl_Position = vec4(aPos, 0.0, 1.0); // Set Z to 0.0 and W to 1.0
}