#version 330 core

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
in mat3 TBN;

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
uniform sampler2D uNormalMap;
uniform float uUseNormalMap;      // 0.0 for false, 1.0 for true

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

// Shadow calculation in world space
float ShadowCalculation(vec3 fragPosWorld) {
    vec3 fragToLight = fragPosWorld - lightPosWorld;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.20;
    int samples = 20;
    float viewDistance = length(cameraPosWorld - fragPosWorld);
    float diskRadius = (1.0 + (viewDistance / farPlane)) / 25.0;

    for(int i = 0; i < samples; ++i)
    {
        // Sample shadow map with a slight offset in world space
        vec3 samplePos = fragToLight + gridSamplingDisk[i] * diskRadius;
        float closestDepth = texture(depthMap, samplePos).r;
        closestDepth *= farPlane; // Undo mapping [0,1]

        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

// Blinn-Phong lighting model for the main light
vec3 blinnPhong(vec3 Kd, vec3 N) {
    vec3 L = normalize(uLightPos_vs - vFragPos); // Light direction in view space
    vec3 V = normalize(-vFragPos); // View direction in view space (camera at origin)
    vec3 H = normalize(L + V); // Halfway vector

    // Distance to the main light
    float distanceToLight = length(uLightPos_vs - vFragPos);

    // Attenuation factors
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    float attenuation = 1.0 / (constant + linear * distanceToLight + quadratic * (distanceToLight * distanceToLight));

    // Diffuse component
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = Kd * uLightIntensity * NdotL * attenuation;

    // Specular component
    float NdotH = max(dot(N, H), 0.0);
    vec3 specular = uKs * uLightIntensity * pow(NdotH, uShininess) * attenuation;

    // Final color contribution from the main light
    return diffuse + specular;
}

void main() {
    // Determine material color (either from texture or uniform)
    vec3 Kd = (uUseTexture > 0.5) ? texture(uTexture, vTexCoords).rgb : uKd;

    // Initialize normal vector
    vec3 N = normalize(vNormal);

    // Sample normal from normal map if enabled
    if (uUseNormalMap > 0.5) {
        vec3 normalMap = texture(uNormalMap, vTexCoords).rgb;
        normalMap = normalMap * 2.0 - 1.0; // Transform from [0,1] to [-1,1]
        N = normalize(TBN * normalMap);     // Transform normal from tangent to view space
    }

    // Calculate the Blinn-Phong lighting model result using the modified normal
    vec3 color = blinnPhong(Kd, N);

    // Define an ambient light component based on material color
    vec3 ambientLight = vec3(0.1);
    vec3 ambient = ambientLight * Kd;

    // Calculate shadow using the ShadowCalculation function in world space
    float shadow = ShadowCalculation(vFragPosWorld);

    // Combine Blinn-Phong color, ambient, and shadow factor
    color = ambient + (1.0 - shadow) * color ;

    // Add additional point lights with Blinn-Phong specular
    vec3 additionalLightsColor = vec3(0.0);

    for (int i = 0; i < uNumAdditionalLights; ++i) {
        // Direction from fragment to this extra light in view space
        vec3 L = normalize(uAdditionalLightPos[i] - vFragPos); // Light direction
        float dist = length(uAdditionalLightPos[i] - vFragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);

        // Diffuse component
        float NdotL = max(dot(N, L), 0.0);
        vec3 diffuse = uAdditionalLightColor[i] * NdotL * uAdditionalLightIntensity[i] * attenuation;

        // Specular component
        vec3 V = normalize(-vFragPos); // View direction in view space
        vec3 H = normalize(L + V); // Halfway vector
        float NdotH = max(dot(N, H), 0.0);
        vec3 specular = uKs * uAdditionalLightColor[i] * pow(NdotH, uShininess) * attenuation;

        // Accumulate diffuse and specular
        additionalLightsColor += diffuse + specular;
    }

    // Add the sum of additional lights to the existing color
    color += additionalLightsColor;

    // Output final fragment color with alpha for transparency
    FragColor = vec4(color, uAlpha);
}