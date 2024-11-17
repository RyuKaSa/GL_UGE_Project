#version 330 core

out vec4 FragColor;

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
};

#define NR_POINT_LIGHTS 10

uniform int numPointLights;
uniform PointLight pointLights[NR_POINT_LIGHTS];

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

void main() {
    vec2 TexCoords = gl_FragCoord.xy / textureSize(gPosition, 0);
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;

    vec3 lighting = vec3(0.0); // Start with no light
    // Ambient lighting
    vec3 ambient = 0.1 * Albedo;
    lighting += ambient;

    // Loop over all point lights
    for(int i = 0; i < numPointLights; i++) {
        // Diffuse shading
        vec3 lightDir = normalize(pointLights[i].position - FragPos);
        float diff = max(dot(Normal, lightDir), 0.0);
        // Attenuation
        float distance = length(pointLights[i].position - FragPos);
        float attenuation = 1.0 / (distance * distance); // Quadratic attenuation

        vec3 diffuse = pointLights[i].color * diff * attenuation * pointLights[i].intensity;

        lighting += diffuse;
    }

    FragColor = vec4(lighting, 1.0);
}