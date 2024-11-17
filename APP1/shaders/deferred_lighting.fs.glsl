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

uniform vec3 cameraPosWorld; // Camera position in world space

// Blinn-Phong parameters
uniform vec3 uKs;          // Specular reflection coefficient
uniform float uShininess;  // Shininess exponent

void main() {
    vec2 TexCoords = gl_FragCoord.xy / textureSize(gPosition, 0);
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
    float SpecularIntensity = texture(gAlbedoSpec, TexCoords).a;

    vec3 lighting = vec3(0.0); // Initialize lighting

    // Ambient term
    vec3 ambient = 0.1 * Albedo;
    lighting += ambient;

    // View direction
    vec3 viewDir = normalize(cameraPosWorld - FragPos);

    // Iterate over all point lights
    for(int i = 0; i < numPointLights; i++) {
        // Light properties
        vec3 lightPos = pointLights[i].position;
        vec3 lightColor = pointLights[i].color;
        float lightIntensity = pointLights[i].intensity;

        // Light direction
        vec3 lightDir = normalize(lightPos - FragPos);

        // Diffuse shading
        float diff = max(dot(Normal, lightDir), 0.0);
        vec3 diffuse = diff * Albedo * lightColor * lightIntensity;

        // Specular shading (Blinn-Phong)
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(Normal, halfwayDir), 0.0), uShininess);
        vec3 specular = uKs * spec * lightColor * lightIntensity * SpecularIntensity;

        // Attenuation
        float distance = length(lightPos - FragPos);
        float attenuation = 1.0 / (distance * distance);
        diffuse *= attenuation;
        specular *= attenuation;

        // Accumulate
        lighting += diffuse + specular;
    }

    FragColor = vec4(lighting, 1.0);
}