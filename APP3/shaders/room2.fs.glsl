#version 330 core

// Maximum number of additional point lights
#define MAX_ADDITIONAL_LIGHTS 100

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

// **Transmission Lighting for Main and Additional Lights**
vec3 TransmissionLighting(vec3 albedo, vec3 N) {
    vec3 transmissionLight = vec3(0.0);

    // Reverse the normal for back-face lighting
    vec3 N_back = -N;

    // --- Main Light Transmission ---
    vec3 L = normalize(uLightPos_vs - vFragPos);
    float distance = length(uLightPos_vs - vFragPos);
    float attenuation = 1.0 / (1.0 + 0.09 * distance + 0.032 * distance * distance);
    float NdotL = max(dot(N_back, L), 0.0); // Use reversed normal
    vec3 mainDiffuse = albedo * uLightIntensity * NdotL * attenuation;
    transmissionLight += mainDiffuse;

    // --- Additional Lights Transmission ---
    for (int i = 0; i < uNumAdditionalLights; ++i) {
        vec3 L_add = normalize(uAdditionalLightPos[i] - vFragPos);
        float distance_add = length(uAdditionalLightPos[i] - vFragPos);
        float attenuation_add = 1.0 / (1.0 + 0.09 * distance_add + 0.032 * distance_add * distance_add);
        float NdotL_add = max(dot(N_back, L_add), 0.0); // Use reversed normal

        vec3 additionalDiffuse = albedo * uAdditionalLightColor[i] * NdotL_add * uAdditionalLightIntensity[i] * attenuation_add;

        transmissionLight += additionalDiffuse;
    }

    // Apply alpha as transmission strength
    return transmissionLight * uAlpha;
}

// **Helper Function: Quantize a Single Color Channel**
float quantizeChannel(float color, int levels) {
    float quantStep = 1.0 / float(levels); // Renamed from 'step' to 'quantStep'
    return floor(color / quantStep + 0.5) * quantStep;
}

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

    // Calculate transmission lighting for both main and additional lights
    vec3 transmissionLighting = TransmissionLighting(albedo, N);

    // Combine all light sources
    vec3 lighting = mainLighting + additionalLighting + transmissionLighting;

    // Sample the texture's color and alpha
    vec4 texColor = texture(uTexture, vTexCoords);

    // Combine texture alpha with uniform alpha
    float finalAlpha = texColor.a * uAlpha;

    // ----------------------------- //
    //       **Color Selection**     //
    // ----------------------------- //
    vec3 colorToDither;

    if (abs(uAlpha - 0.9) < 0.001) {
        // **Special Case: Flat Texture Color for alpha == 0.9**
        colorToDither = albedo;
    } else {
        // **Normal Case: Combined Lighting**
        colorToDither = lighting * texColor.rgb;
    }

    // ----------------------------- //
    //       **Dithering Effect**    //
    // ----------------------------- //
    
    // ----- **Dither Configuration (Enhanced)** -----
    const bool ENABLE_DITHER = true;          // Toggle dithering: true to enable, false to disable
    const bool MONOCHROME_DITHER = false;     // Toggle monochromatic dithering: true for monochrome, false for color
    const int COLOR_LEVELS = 4;               // Reduced for more pronounced dithering

    // 4x3 Bayer-Like Matrix (Normalized)
    const float bayerMatrix[12] = float[12](
        // Row 0
        0.0/12.0, 6.0/12.0, 2.0/12.0, 4.0/12.0,
        // Row 1
        3.0/12.0, 9.0/12.0, 1.0/12.0, 7.0/12.0,
        // Row 2
        5.0/12.0, 11.0/12.0, 3.0/12.0, 10.0/12.0
    );

    vec3 ditheredColor = colorToDither;

    if (ENABLE_DITHER) {
        // Calculate the local position within the 4x3 big pixel
        int localX = int(mod(gl_FragCoord.x, 4.0));
        int localY = int(mod(gl_FragCoord.y, 3.0));
        
        // Calculate index for 4x3 matrix (12 elements)
        int index = localY * 4 + localX; // 4 columns per row

        // Clamp index to prevent out-of-bounds access (safety measure)
        index = clamp(index, 0, 11);

        float threshold = bayerMatrix[index];

        // Declare quantStep outside the loop to ensure it's in scope for MONOCHROME_DITHER
        float quantStep = 1.0 / float(COLOR_LEVELS);

        for(int i = 0; i < 3; ++i) { // Iterate over R, G, B
            float channel = ditheredColor[i];
            float quantized = floor(channel / quantStep) * quantStep;
            
            // Compare channel value with threshold to decide quantization
            if (fract(channel / quantStep) > threshold) {
                ditheredColor[i] = quantized + quantStep;
            } else {
                ditheredColor[i] = quantized;
            }
        }
        
        if (MONOCHROME_DITHER) {
            const vec3 MONOCHROME_COLOR = vec3(0.95, 0.68, 0.49); 
            float luminance = dot(ditheredColor, vec3(0.299, 0.587, 0.114));
            float quantizedLuminance = floor(luminance / quantStep) * quantStep;
            if (fract(luminance / quantStep) > threshold) {
                quantizedLuminance += quantStep;
            }
            ditheredColor = MONOCHROME_COLOR * quantizedLuminance;
        }
    }

    // ----------------------------- //
    //       **Old Television Effect**//
    // ----------------------------- //
    
    // **Old Television Effect: Column-Based Masking**
    const int CYCLE_SIZE = 4;

    // Calculate the current column index based on fragment coordinates
    int column = int(mod(gl_FragCoord.x, float(CYCLE_SIZE)));

    // Initialize channel mask to full color
    vec3 channelMask = vec3(1.0, 1.0, 1.0);

    // Assign specific color channels based on column
    if(column == 0){
        channelMask = vec3(1.0, 0.3, 0.3); // Red dominant
    }
    else if(column == 1){
        channelMask = vec3(0.3, 1.0, 0.3); // Green dominant
    }
    else if(column == 2){
        channelMask = vec3(0.3, 0.3, 1.0); // Blue dominant
    }
    else if(column == 3){
        channelMask = vec3(0.0, 0.0, 0.0); // Black line
    }

    // Apply the channel mask to the dithered color
    vec3 finalColor = ditheredColor * channelMask;

    // **Row-Based Black Lines: Add a black line every third row**
    const int ROW_CYCLE_SIZE = 3;

    // Calculate the current row index based on fragment coordinates
    int row = int(mod(gl_FragCoord.y, float(ROW_CYCLE_SIZE)));

    // If the current row is the first in the cycle, blend the color towards black
    if(row == 0){
        finalColor = mix(finalColor, vec3(0.0, 0.0, 0.0), 0.7); // Blend 70% towards black
    }

    // ----------------------------- //
    //       **Final Output**        //
    // ----------------------------- //
    
    FragColor = vec4(finalColor, finalAlpha);
}