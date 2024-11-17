#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

// Remove texture sampling for simplicity
// uniform sampler2D texture_diffuse;

void main() {
    gPosition = FragPos;
    gNormal = normalize(Normal);
    gAlbedoSpec.rgb = vec3(1.0, 1.0, 1.0); // Set albedo to white
    gAlbedoSpec.a = 1.0; // Specular intensity placeholder
}