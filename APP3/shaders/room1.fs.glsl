#version 330 core

// Maximum number of additional point lights
#define MAX_ADDITIONAL_LIGHTS 8

// Uniforms for additional point lights
uniform int uNumAdditionalLights;
uniform vec3 uAdditionalLightPos[MAX_ADDITIONAL_LIGHTS];      
uniform vec3 uAdditionalLightColor[MAX_ADDITIONAL_LIGHTS];
uniform float uAdditionalLightIntensity[MAX_ADDITIONAL_LIGHTS];

// Input from vertex shader
in vec3 vNormal;
in vec3 vFragPos;         
in vec3 vFragPosWorld;    
in vec2 vTexCoords;
in mat3 TBN;

out vec4 FragColor;

// Material properties
uniform vec3 uKd;           
uniform vec3 uKs;           
uniform float uShininess;   

// Transparency
uniform float uAlpha;

// Main light properties (in view space)
uniform vec3 uLightPos_vs;    
uniform vec3 uLightIntensity; 
uniform float farPlane;

// Camera position in world space
uniform vec3 cameraPosWorld;

// Texture samplers
uniform sampler2D uTexture;
uniform float uUseTexture;        

// Normal map
uniform sampler2D uNormalMap;
uniform float uUseNormalMap;

// Specular map
uniform sampler2D uSpecularMap;
uniform float uUseSpecularMap;

// Shadow mapping
uniform samplerCube depthMap;

// Light position in world space
uniform vec3 lightPosWorld;

// Hardcoded map strengths
const float NORMAL_MAP_STRENGTH = 0.3;
const float SPECULAR_MAP_STRENGTH = 3.0;

// Sampling offsets for shadow mapping
const vec3 gridSamplingDisk[20] = vec3[](
    vec3( 1,  0,  0), vec3(-1,  0,  0), vec3( 0,  1,  0), vec3( 0, -1,  0), 
    vec3( 0,  0,  1), vec3( 0,  0, -1), vec3( 1,  1,  0), vec3(-1,  1,  0), 
    vec3( 1, -1,  0), vec3(-1, -1,  0), vec3( 1,  0,  1), vec3(-1,  0,  1), 
    vec3( 1,  0, -1), vec3(-1,  0, -1), vec3( 0,  1,  1), vec3( 0, -1,  1), 
    vec3( 0,  1, -1), vec3( 0, -1, -1), vec3( 1,  1,  1), vec3(-1, -1, -1)
);

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
        vec3 samplePos = fragToLight + gridSamplingDisk[i] * diskRadius;
        float closestDepth = texture(depthMap, samplePos).r * farPlane;

        if (currentDepth - bias > closestDepth) {
            shadow += 1.0;
        }
    }
    shadow /= float(samples);
    return shadow;
}

// **Normal Map Sampling with Strength**
vec3 GetNormalFromMap(vec3 defaultNormal) {
    vec3 normalMap = texture(uNormalMap, vTexCoords).rgb;
    normalMap = normalMap * 2.0 - 1.0; // Transform from [0,1] to [-1,1]

    // Blend geometry normal and normal map using strength
    return normalize(mix(defaultNormal, TBN * normalMap, NORMAL_MAP_STRENGTH));
}

// **Specular Intensity from Map with Strength**
float GetSpecularIntensity() {
    if (uUseSpecularMap > 0.5) {
        return texture(uSpecularMap, vTexCoords).r * SPECULAR_MAP_STRENGTH; // Scale with strength
    }
    return 1.0 * SPECULAR_MAP_STRENGTH; // Default intensity with strength
}

// **Main Light - Diffuse**
vec3 MainLightDiffuse(vec3 albedo, vec3 N) {
    vec3 L = normalize(uLightPos_vs - vFragPos);
    float distance = length(uLightPos_vs - vFragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    float NdotL = max(dot(N, L), 0.0);
    return albedo * uLightIntensity * NdotL * attenuation;
}

// **Main Light - Specular**
vec3 MainLightSpecular(vec3 N) {
    vec3 L = normalize(uLightPos_vs - vFragPos);
    vec3 V = normalize(-vFragPos); 
    vec3 H = normalize(L + V);

    float distance = length(uLightPos_vs - vFragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

    float NdotH = max(dot(N, H), 0.0);
    float specularIntensity = GetSpecularIntensity();
    return uKs * specularIntensity * uLightIntensity * pow(NdotH, uShininess) * attenuation;
}

// **Additional Lights**
vec3 AdditionalLights(vec3 albedo, vec3 N) {
    vec3 totalLight = vec3(0.0);

    for (int i = 0; i < uNumAdditionalLights; ++i) {
        vec3 L = normalize(uAdditionalLightPos[i] - vFragPos);
        vec3 V = normalize(-vFragPos); 
        vec3 H = normalize(L + V);

        float distance = length(uAdditionalLightPos[i] - vFragPos);
        float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);

        float NdotL = max(dot(N, L), 0.0);
        float NdotH = max(dot(N, H), 0.0);

        float specularIntensity = GetSpecularIntensity();

        vec3 diffuse = albedo * uAdditionalLightColor[i] * NdotL * uAdditionalLightIntensity[i] * attenuation;
        vec3 specular = uKs * specularIntensity * uAdditionalLightColor[i] * pow(NdotH, uShininess) * uAdditionalLightIntensity[i] * attenuation;

        totalLight += diffuse + specular;
    }

    return totalLight;
}

// **Omni-Directional Lighting for Transparency**
vec3 CalculateOmniDirectionalLighting(vec3 albedo, vec3 N) {
    vec3 omniLight = vec3(0.0);

    // Main Light Contribution
    float distance = length(uLightPos_vs - vFragPos);
    float attenuation = 1.0 / (1.0 + 0.05 * distance + 0.01 * distance * distance);
    vec3 L = normalize(uLightPos_vs - vFragPos);

    float NdotL = max(dot(N, L), 0.0);       
    float NdotL_inv = max(dot(-N, L), 0.0);  

    // Reduce weight for side directions
    float weight = (NdotL > 0.5 || NdotL_inv > 0.5) ? 1.0 : 0.5; 

    omniLight += albedo * uLightIntensity * attenuation * weight;

    // Additional Lights Contribution
    for (int i = 0; i < uNumAdditionalLights; ++i) {
        float distance_add = length(uAdditionalLightPos[i] - vFragPos);
        float attenuation_add = 1.0 / (1.0 + 0.05 * distance_add + 0.01 * distance_add * distance_add);
        vec3 L_add = normalize(uAdditionalLightPos[i] - vFragPos);

        float NdotL_add = max(dot(N, L_add), 0.0);
        float NdotL_inv_add = max(dot(-N, L_add), 0.0);

        float weight_add = (NdotL_add > 0.5 || NdotL_inv_add > 0.5) ? 1.0 : 0.5; 

        omniLight += albedo * uAdditionalLightColor[i] * uAdditionalLightIntensity[i] * attenuation_add * weight_add;
    }

    return omniLight * uAlpha;
}

// **Fragment Shader Main Function**
void main() {
    // Determine albedo based on whether a diffuse texture is used
    vec3 albedo = (uUseTexture > 0.5) ? texture(uTexture, vTexCoords).rgb : uKd;
    
    // Determine normal based on whether a normal map is used
    vec3 N = (uUseNormalMap > 0.5) ? GetNormalFromMap(normalize(vNormal)) : normalize(vNormal);

    // Sample the texture's color and alpha
    vec4 texColor = texture(uTexture, vTexCoords);
    float finalAlpha = texColor.a * uAlpha;

    vec3 lighting = vec3(0.0);

    // Check if material is transparent
    if (uAlpha < 1.0) {
        // Transparent Material: Omni-Directional Lighting
        lighting = CalculateOmniDirectionalLighting(albedo, N);
    } else {
        // Opaque Material: Standard Lighting with Shadows
        float shadow = ShadowCalculation(vFragPosWorld);

        // Calculate main light's lighting contributions
        vec3 mainDiffuse = MainLightDiffuse(albedo, N);
        vec3 mainSpecular = MainLightSpecular(N);
        vec3 mainLighting = (mainDiffuse + mainSpecular) * (1.0 - shadow);

        // Calculate additional lights' lighting contributions
        vec3 additionalLighting = AdditionalLights(albedo, N);

        // Combine all light sources
        lighting = mainLighting + additionalLighting;
    }

    // Set the final fragment color with combined alpha
    FragColor = vec4(lighting * texColor.rgb, finalAlpha);
}