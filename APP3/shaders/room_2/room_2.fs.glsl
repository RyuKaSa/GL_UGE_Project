#version 330 core
layout (location = 0) out vec3 FragPosition; // Output position to G-buffer
layout (location = 1) out vec3 FragNormal; // Output normal to G-buffer
layout (location = 2) out vec4 FragAlbedoSpec; // Output albedo (RGB) + specular (A)

in vec3 FragPos; // From vertex shader
in vec3 Normal; // From vertex shader
in vec2 TexCoords; // From vertex shader

uniform sampler2D diffuseTexture; // Diffuse texture

void main()
{
    // Write world-space position
    FragPosition = FragPos;

    // Write normalized world-space normal
    FragNormal = normalize(Normal);

    // Write albedo color and specular intensity (alpha channel)
    vec3 albedo = texture(diffuseTexture, TexCoords).rgb;
    float specular = 1.0; // Constant specular value for simplicity; can be changed
    FragAlbedoSpec = vec4(albedo, specular);
}