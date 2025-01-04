#version 330 core

// ---------------------------------------------
//              UNIFORMS & CONSTANTS
// ---------------------------------------------

#define MAX_ADDITIONAL_LIGHTS 8

// Additional Lights
uniform int uNumAdditionalLights;
uniform vec3 uAdditionalLightPos[MAX_ADDITIONAL_LIGHTS];      
uniform vec3 uAdditionalLightColor[MAX_ADDITIONAL_LIGHTS];
uniform float uAdditionalLightIntensity[MAX_ADDITIONAL_LIGHTS];

// Lighting and Materials
uniform vec3 uKd;           
uniform vec3 uKs;           
uniform float uShininess;   
uniform float uAlpha;

// Main Light
uniform vec3 uLightPos_vs;    
uniform vec3 uLightIntensity; 
uniform float farPlane;

// Camera
uniform vec3 cameraPosWorld;

// Textures
uniform sampler2D uTexture;
uniform float uUseTexture;        
uniform sampler2D uNormalMap;
uniform float uUseNormalMap;
uniform sampler2D uSpecularMap;
uniform float uUseSpecularMap;

// Shadow Mapping
uniform samplerCube depthMap;
uniform vec3 lightPosWorld;

// Hardcoded Map Strengths
const float NORMAL_MAP_STRENGTH = 0.3;
const float SPECULAR_MAP_STRENGTH = 3.0;

// Dithering
const bool ENABLE_DITHER = true;
const bool MONOCHROME_DITHER = false;
const int COLOR_LEVELS = 8;

// Input from Vertex Shader
in vec3 vNormal;
in vec3 vFragPos;         
in vec3 vFragPosWorld;    
in vec2 vTexCoords;
in mat3 TBN;

// Final Output
out vec4 FragColor;

// ---------------------------------------------
//              UTILITY FUNCTIONS
// ---------------------------------------------

// **Shadow Calculation**
float ShadowCalculation(vec3 fragPosWorld) {
    vec3 fragToLight = fragPosWorld - lightPosWorld;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.25; 
    int samples = 20;
    float viewDistance = length(cameraPosWorld - fragPosWorld);
    float diskRadius = (0.0 + (viewDistance / farPlane)) / 50.0;

    for (int i = 0; i < samples; ++i) {
        vec3 samplePos = fragToLight + diskRadius * vec3(
            fract(sin(i * 12.9898) * 43758.5453),
            fract(sin(i * 78.233) * 12345.6789),
            fract(sin(i * 45.678) * 98765.4321)
        );
        float closestDepth = texture(depthMap, samplePos).r * farPlane;

        if (currentDepth - bias > closestDepth) {
            shadow += 1.0;
        }
    }
    return shadow / float(samples);
}

// **Normal Map Sampling with Strength**
vec3 GetNormalFromMap(vec3 defaultNormal) {
    vec3 normalMap = texture(uNormalMap, vTexCoords).rgb;
    normalMap = normalMap * 2.0 - 1.0; // Transform from [0,1] to [-1,1]
    return normalize(mix(defaultNormal, TBN * normalMap, NORMAL_MAP_STRENGTH));
}

// **Specular Intensity from Map**
float GetSpecularIntensity() {
    if (uUseSpecularMap > 0.5) {
        return texture(uSpecularMap, vTexCoords).r * SPECULAR_MAP_STRENGTH;
    }
    return 1.0 * SPECULAR_MAP_STRENGTH;
}

// **Main Light Contribution**
vec3 CalculateMainLight(vec3 albedo, vec3 N) {
    vec3 L = normalize(uLightPos_vs - vFragPos);
    float distance = length(uLightPos_vs - vFragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    
    float NdotL = max(dot(N, L), 0.0);
    float NdotH = max(dot(N, normalize(L + normalize(-vFragPos))), 0.0);
    
    vec3 diffuse = albedo * uLightIntensity * NdotL * attenuation;
    vec3 specular = uKs * GetSpecularIntensity() * uLightIntensity * pow(NdotH, uShininess) * attenuation;

    return diffuse + specular;
}

// **Additional Lights Contribution**
vec3 CalculateAdditionalLights(vec3 albedo, vec3 N) {
    vec3 totalLight = vec3(0.0);
    for (int i = 0; i < uNumAdditionalLights; ++i) {
        vec3 L = normalize(uAdditionalLightPos[i] - vFragPos);
        float distance = length(uAdditionalLightPos[i] - vFragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
        float NdotL = max(dot(N, L), 0.0);
        float NdotH = max(dot(N, normalize(L + normalize(-vFragPos))), 0.0);

        vec3 diffuse = albedo * uAdditionalLightColor[i] * NdotL * uAdditionalLightIntensity[i] * attenuation;
        vec3 specular = uKs * GetSpecularIntensity() * uAdditionalLightColor[i] * pow(NdotH, uShininess) * uAdditionalLightIntensity[i] * attenuation;

        totalLight += diffuse + specular;
    }
    return totalLight;
}

// **Transmission Lighting**
vec3 CalculateTransmissionLighting(vec3 albedo, vec3 N) {
    return (CalculateMainLight(albedo, -N) + CalculateAdditionalLights(albedo, -N)) * uAlpha;
}

// **Dither Effect (with Scaling and Monochrome Option)**
vec3 ApplyDither(vec3 color) {
    // 4x4 Bayer Matrix (Normalized)
    const float bayerMatrix[16] = float[16](
        0.0/16.0, 8.0/16.0, 2.0/16.0, 10.0/16.0,
        12.0/16.0, 4.0/16.0, 14.0/16.0, 6.0/16.0,
        3.0/16.0, 11.0/16.0, 1.0/16.0, 9.0/16.0,
        15.0/16.0, 7.0/16.0, 13.0/16.0, 5.0/16.0
    );

    float scaledX = gl_FragCoord.x / 4.0; 
    float scaledY = gl_FragCoord.y / 4.0;
    int x = int(mod(scaledX, 4.0));
    int y = int(mod(scaledY, 4.0));
    int index = y * 4 + x;
    float threshold = bayerMatrix[index];

    for (int i = 0; i < 3; ++i) {
        float channel = color[i];
        color[i] = floor(channel * COLOR_LEVELS + threshold) / COLOR_LEVELS;
    }

    if (MONOCHROME_DITHER) {
        const vec3 MONOCHROME_COLOR = vec3(0.95, 0.68, 0.49); 
        float luminance = dot(color, vec3(0.299, 0.587, 0.114));
        color = MONOCHROME_COLOR * luminance;
    }

    return color;
}

// ---------------------------------------------
//              MAIN FUNCTION
// ---------------------------------------------

void main() {
    vec3 albedo = (uUseTexture > 0.5) ? texture(uTexture, vTexCoords).rgb : uKd;
    vec3 N = (uUseNormalMap > 0.5) ? GetNormalFromMap(normalize(vNormal)) : normalize(vNormal);

    float shadow = ShadowCalculation(vFragPosWorld);
    vec3 lighting = (CalculateMainLight(albedo, N) * (1.0 - shadow)) 
                    + CalculateAdditionalLights(albedo, N) 
                    + CalculateTransmissionLighting(albedo, N);

    vec4 texColor = texture(uTexture, vTexCoords);
    float finalAlpha = texColor.a * uAlpha;

    if (ENABLE_DITHER) {
        lighting = ApplyDither(lighting);
    }

    FragColor = vec4(lighting * texColor.rgb, finalAlpha);
}