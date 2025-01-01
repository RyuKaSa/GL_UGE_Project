#version 330 core

// Maximum number of additional point lights
#define MAX_ADDITIONAL_LIGHTS 4

// Uniforms for additional point lights
uniform int uNumAdditionalLights;
uniform vec3 uAdditionalLightPos[MAX_ADDITIONAL_LIGHTS];      // In view space
uniform vec3 uAdditionalLightColor[MAX_ADDITIONAL_LIGHTS];
uniform float uAdditionalLightIntensity[MAX_ADDITIONAL_LIGHTS];

// Input from vertex shader
in vec3 vNormal;
in vec3 vFragPos;         // In view space
in vec3 vFragPosWorld;    // In world space
in vec2 vTexCoords;

out vec4 FragColor;

// Material properties
uniform vec3 uKd;           // Diffuse reflection coefficient
uniform vec3 uKs;           // Specular reflection coefficient
uniform float uShininess;   // Shininess exponent

// Transparency
uniform float uAlpha;

// Main light properties (in view space)
uniform vec3 uLightPos_vs;    // Main light position in view space
uniform vec3 uLightIntensity; // Main light intensity (color)
uniform float farPlane;

// Camera position in world space
uniform vec3 cameraPosWorld;

// Texture samplers
uniform sampler2D uTexture;
uniform float uUseTexture;        // 0.0 for false, 1.0 for true

// Shadow mapping
uniform samplerCube depthMap;

// Light position in world space
uniform vec3 lightPosWorld;

// Sampling offsets for shadow mapping
const vec3 gridSamplingDisk[20] = vec3[](
    vec3( 1,  0,  0), vec3(-1,  0,  0), vec3( 0,  1,  0), vec3( 0, -1,  0), 
    vec3( 0,  0,  1), vec3( 0,  0, -1), vec3( 1,  1,  0), vec3(-1,  1,  0), 
    vec3( 1, -1,  0), vec3(-1, -1,  0), vec3( 1,  0,  1), vec3(-1,  0,  1), 
    vec3( 1,  0, -1), vec3(-1,  0, -1), vec3( 0,  1,  1), vec3( 0, -1,  1), 
    vec3( 0,  1, -1), vec3( 0, -1, -1), vec3( 1,  1,  1), vec3(-1, -1, -1)
);

// 🛡️ **Shadow Calculation**
float ShadowCalculation(vec3 fragPosWorld) {
    vec3 fragToLight = fragPosWorld - lightPosWorld;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.25; 
    int samples = 20;
    float viewDistance = length(cameraPosWorld - fragPosWorld);
    float diskRadius = (0.0 + (viewDistance / farPlane)) / 50.0;

    for(int i = 0; i < samples; ++i) {
        vec3 samplePos = fragToLight + gridSamplingDisk[i] * diskRadius;
        float closestDepth = texture(depthMap, samplePos).r * farPlane;

        if(currentDepth - bias > closestDepth) {
            shadow += 1.0;
        }
    }
    shadow /= float(samples);
    return shadow;
}

// 💡 **Diffuse Component (Lambertian Lighting)**
vec3 MainLightDiffuse(vec3 albedo, vec3 N) {
    vec3 L = normalize(uLightPos_vs - vFragPos); // Light direction in view space

    // Distance attenuation
    float distance = length(uLightPos_vs - vFragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    // Lambertian Diffuse Shading
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = albedo * uLightIntensity * NdotL * attenuation;

    return diffuse;
}

// ✨ **Specular Component (Blinn-Phong Reflection Model)**
vec3 MainLightSpecular(vec3 N) {
    vec3 L = normalize(uLightPos_vs - vFragPos); // Light direction in view space
    vec3 V = normalize(-vFragPos); // View direction (camera at origin)
    vec3 H = normalize(L + V); // Halfway vector

    float distance = length(uLightPos_vs - vFragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    float NdotH = max(dot(N, H), 0.0);
    vec3 specular = uKs * uLightIntensity * pow(NdotH, uShininess) * attenuation;

    return specular;
}

// 🎨 **Lighting Calculation**
vec3 CalculateLighting(vec3 albedo, vec3 N) {
    // Diffuse
    vec3 diffuse = MainLightDiffuse(albedo, N);

    // Specular
    vec3 specular = MainLightSpecular(N);

    // Combine Diffuse and Specular
    return diffuse + specular;
}

// 🖌️ **Fragment Shader Main Function**
void main() {
    // Sample texture color or fallback to uniform Kd
    vec3 albedo = (uUseTexture > 0.5) ? texture(uTexture, vTexCoords).rgb : uKd;

    // Initialize normal vector from geometry
    vec3 N = normalize(vNormal);

    // Calculate shadow
    float shadow = ShadowCalculation(vFragPosWorld);

    // Apply lighting calculations
    vec3 lighting = CalculateLighting(albedo, N);

    // Apply shadow to final lighting
    lighting *= (1.0 - shadow);

    // Final color with transparency
    FragColor = vec4(lighting, uAlpha);
}