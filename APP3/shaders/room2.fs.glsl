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
const float NORMAL_MAP_STRENGTH = 0.0;
const float SPECULAR_MAP_STRENGTH = 0.0;
// we can simplify the dither effect by removing the normal map and specular map strengths, (though i'll leave them in, at 0.0)

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

// **Helper Function: Quantize a Single Color Channel**
float quantizeChannel(float color, int levels) {
    float step = 1.0 / float(levels);
    return floor(color / step + 0.5) * step;
}

// **Fragment Shader Main Function**
void main() {
    // Determine albedo based on whether a diffuse texture is used
    vec3 albedo = (uUseTexture > 0.5) ? texture(uTexture, vTexCoords).rgb : uKd;
    
    // Determine normal based on whether a normal map is used
    vec3 N = (uUseNormalMap > 0.5) ? GetNormalFromMap(normalize(vNormal)) : normalize(vNormal);

    // Calculate shadow factor based on main light's shadow
    float shadow = ShadowCalculation(vFragPosWorld);

    // Calculate main light's lighting contributions
    vec3 mainDiffuse = MainLightDiffuse(albedo, N);
    vec3 mainSpecular = MainLightSpecular(N);
    vec3 mainLighting = (mainDiffuse + mainSpecular) * (1.0 - shadow);

    // Calculate additional lights' lighting contributions
    vec3 additionalLighting = AdditionalLights(albedo, N);

    // Combine main lighting and additional lighting
    vec3 lighting = mainLighting + additionalLighting;

    // Sample the texture's color and alpha
    vec4 texColor = texture(uTexture, vTexCoords);

    // Combine texture alpha with uniform alpha
    float finalAlpha = texColor.a * uAlpha;

    // ----------------------------- //
    //       **Dithering Effect**    //
    // ----------------------------- //

    // ----- **Dither Configuration (Hard-Coded)** -----
    const bool ENABLE_DITHER = true;          // Toggle dithering: true to enable, false to disable
    const bool MONOCHROME_DITHER = false;     // Toggle monochromatic dithering: true for monochrome, false for color
    const int COLOR_LEVELS = 8;               // Number of color levels per channel (e.g., 8 for 3-bit color)

    // 4x4 Bayer Matrix (Normalized)
    const float bayerMatrix[16] = float[16](
        0.0/16.0,  8.0/16.0,  2.0/16.0, 10.0/16.0,
        12.0/16.0, 4.0/16.0, 14.0/16.0,  6.0/16.0,
        3.0/16.0, 11.0/16.0,  1.0/16.0,  9.0/16.0,
        15.0/16.0, 7.0/16.0, 13.0/16.0,  5.0/16.0
    );

    if (ENABLE_DITHER) {
        // Calculate fragment's position within the 4x4 Bayer matrix
        // Mapping multiple pixels per pattern for better coverage
        // Adjust scaling factor as needed to control pattern density
        float scaledX = gl_FragCoord.x / 4.0; // Scale down to map multiple pixels per pattern
        float scaledY = gl_FragCoord.y / 4.0;
        int x = int(mod(scaledX, 4.0));
        int y = int(mod(scaledY, 4.0));
        int index = y * 4 + x;

        float threshold = bayerMatrix[index];

        // Apply dithering to RGB channels
        vec3 ditheredColor = lighting * texColor.rgb;

        for(int i = 0; i < 3; ++i) { // Iterate over R, G, B
            float channel = ditheredColor[i];
            float quantized = quantizeChannel(channel, COLOR_LEVELS);
            float error = fract(channel * float(COLOR_LEVELS)) - 0.5;

            if (error > (threshold - 0.5)) {
                ditheredColor[i] = quantized + (1.0 / float(COLOR_LEVELS));
            } else {
                ditheredColor[i] = quantized;
            }
        }

        // Apply monochromatic dithering if enabled
        if (MONOCHROME_DITHER) {
            // Hardcoded monochromatic color
            const vec3 MONOCHROME_COLOR = vec3((0.95), (0.68), (0.49)); // i like how beige looks on mono chroma dithering

            // Convert to grayscale using luminance
            float luminance = dot(ditheredColor, vec3(0.299, 0.587, 0.114));
            float quantizedLuminance = quantizeChannel(luminance + threshold / float(COLOR_LEVELS), COLOR_LEVELS);
            
            // Map the quantized luminance to the hardcoded color
            ditheredColor = MONOCHROME_COLOR * quantizedLuminance;
        }

        // Update the finalColor with ditheredColor
        FragColor = vec4(ditheredColor, finalAlpha);
    } else {
        // If dithering is disabled, render without dither
        FragColor = vec4(lighting * texColor.rgb, finalAlpha);
    }
}