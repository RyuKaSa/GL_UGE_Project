#version 330 core

in vec3 FragPosWorld;
in vec3 NormalWorld;
in vec2 TexCoords;

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gAlbedoSpec;

void main() {
    gPosition = FragPosWorld;
    gNormal = normalize(NormalWorld);
    gAlbedoSpec.rgb = vec3(1.0, 1.0, 1.0); // Albedo
    gAlbedoSpec.a = 1.0; // Specular intensity
}