#version 330 core

in vec3 vNormal;
in vec3 vFragPos;
in vec2 vTexCoords;

out vec4 FragColor;

// Material properties
uniform vec3 uKd;         // Diffuse reflection coefficient
uniform vec3 uKs;         // Specular reflection coefficient
uniform float uShininess; // Shininess exponent

// Light properties
uniform vec3 uLightDir_vs;      // Light direction in view space
uniform vec3 uLightIntensity;   // Light intensity (color)

// Texture sampler
uniform sampler2D uTexture;
uniform float uUseTexture; // Use 0.0 for false, 1.0 for true

vec3 blinnPhong(vec3 Kd)
{
    // Normalize vectors
    vec3 N = normalize(vNormal);
    vec3 L = normalize(-uLightDir_vs);
    vec3 V = normalize(-vFragPos);
    vec3 H = normalize(L + V);

    // Diffuse term
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = Kd * uLightIntensity * NdotL;

    // Specular term
    float NdotH = max(dot(N, H), 0.0);
    vec3 specular = uKs * uLightIntensity * pow(NdotH, uShininess);

    // Final color
    return diffuse + specular;
}

void main()
{
    vec3 Kd;
    if (uUseTexture > 0.5)
    {
        Kd = texture(uTexture, vTexCoords).rgb;
    }
    else
    {
        Kd = uKd;
    }

    vec3 color = blinnPhong(Kd);
    FragColor = vec4(color, 1.0);
}