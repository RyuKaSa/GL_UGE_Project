#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec3 Albedo;

layout(location = 0) out vec3 gPosition;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec3 gAlbedoSpec;

void main() {
    gPosition = FragPos;                   // Pass position
    gNormal = normalize(Normal);           // Pass normalized normal vector
    gAlbedoSpec.rgb = Albedo;              // Pass color as albedo
}