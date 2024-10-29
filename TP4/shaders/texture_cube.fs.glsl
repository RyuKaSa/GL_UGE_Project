#version 330 core

in vec3 vNormal;
in vec3 vFragPos;
in vec2 vTexCoords;

out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    // Sample the texture
    vec3 textureColor = texture(uTexture, vTexCoords).rgb;

    // For simplicity, we'll just use the texture color directly
    FragColor = vec4(textureColor, 1.0);
}