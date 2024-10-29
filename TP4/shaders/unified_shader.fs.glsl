#version 330 core

in vec3 vNormal;
in vec3 vFragPos;
in vec2 vTexCoords;

out vec4 FragColor;

uniform sampler2D uTexture;
uniform vec3 uObjectColor;
uniform float uUseTexture; // Use 0.0 for false, 1.0 for true

void main()
{
    // Sample the texture
    vec3 textureColor = texture(uTexture, vTexCoords).rgb;
    
    // Mix the object color and texture color based on uUseTexture
    vec3 color = mix(uObjectColor, textureColor, uUseTexture);
    
    // Simple diffuse lighting
    vec3 lightDir = normalize(vec3(0.0, 1.0, 1.0));
    float diff = max(dot(normalize(vNormal), lightDir), 0.0);
    vec3 diffuse = diff * color;

    FragColor = vec4(diffuse, 1.0);
}