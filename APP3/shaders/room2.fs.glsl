#version 330 core

// simple point lights
#define MAX_ADDITIONAL_LIGHTS 4

uniform int uNumAdditionalLights;
uniform vec3 uAdditionalLightPos[MAX_ADDITIONAL_LIGHTS];
uniform vec3 uAdditionalLightColor[MAX_ADDITIONAL_LIGHTS];
uniform float uAdditionalLightIntensity[MAX_ADDITIONAL_LIGHTS];

in vec3 vNormal;
in vec3 vFragPos;
in vec3 vFragPosWorld;
in vec2 vTexCoords;
in mat3 TBN;

out vec4 FragColor;

// Material properties
uniform vec3 uKd;         // Diffuse reflection coefficient
uniform vec3 uKs;         // Specular reflection coefficient
uniform float uShininess; // Shininess exponent

// Light properties
uniform vec3 uLightPos_vs; // Light position in view space
uniform vec3 lightPosWorld;    // Light position in world space
uniform vec3 uLightIntensity;  // Light intensity (color)
uniform float farPlane;

// Camera position
uniform vec3 cameraPosWorld;

// Texture sampler
uniform sampler2D uTexture;
uniform float uUseTexture; // Use 0.0 for false, 1.0 for true

// Normal map
uniform sampler2D uNormalMap;
uniform float uUseNormalMap; // Use 0.0 for false, 1.0 for true

// Depth cube map
uniform samplerCube depthMap;

const vec3 gridSamplingDisk[20] = vec3[](
    vec3( 1,  0,  0), vec3(-1,  0,  0), vec3( 0,  1,  0), vec3( 0, -1,  0), 
    vec3( 0,  0,  1), vec3( 0,  0, -1), vec3( 1,  1,  0), vec3(-1,  1,  0), 
    vec3( 1, -1,  0), vec3(-1, -1,  0), vec3( 1,  0,  1), vec3(-1,  0,  1), 
    vec3( 1,  0, -1), vec3(-1,  0, -1), vec3( 0,  1,  1), vec3( 0, -1,  1), 
    vec3( 0,  1, -1), vec3( 0, -1, -1), vec3( 1,  1,  1), vec3(-1, -1, -1)
);

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
        // Sample shadow map with a slight offset
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= farPlane; // Undo mapping [0,1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

vec3 blinnPhong(vec3 Kd, vec3 N) {
    vec3 L = normalize(uLightPos_vs - vFragPos); // Light direction in view space
    vec3 V = normalize(-vFragPos); // View direction in view space (camera at origin)
    vec3 H = normalize(L + V); // Halfway vector

    // Distance to the light
    float distanceToLight = length(uLightPos_vs - vFragPos);

    // Attenuation factors
    float constant = 1.0;
    float linear = 0.09;
    float quadratic = 0.032;
    float attenuation = 1.0 / (constant + linear * distanceToLight + quadratic * (distanceToLight * distanceToLight));

    // Apply dynamic light color to diffuse component
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = Kd * uLightIntensity * NdotL * attenuation;

    // Apply dynamic light color to specular component
    float NdotH = max(dot(N, H), 0.0);
    vec3 specular = uKs * uLightIntensity * pow(NdotH, uShininess) * attenuation;

    // Final color
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

    // Calculate shadow using the ShadowCalculation function
    float shadow = ShadowCalculation(vFragPosWorld);

    // Combine Blinn-Phong color, ambient, and shadow factor
    color = ambient + (1.0 - shadow) * color ;

    // Add additional point lights
    vec3 additionalLightsColor = vec3(0.0);

    for (int i = 0; i < uNumAdditionalLights; ++i) {
        // Direction from fragment to this extra light in view space
        vec3 toLight = uAdditionalLightPos[i] - vFragPos; // Both in view space
        float dist = length(toLight);
        vec3 L = normalize(toLight);

        // Lambertian diffuse
        float NdotL = max(dot(N, L), 0.0);

        // Attenuation factors
        float attenuation = 1.0 / (1.0 + 0.09 * dist + 0.032 * dist * dist);

        // Diffuse component
        vec3 diffuse = uAdditionalLightColor[i] * NdotL * uAdditionalLightIntensity[i] * attenuation;

        additionalLightsColor += diffuse;
    }

    // Add the sum of extra lights to the existing color
    color += additionalLightsColor;

    // room 2 specific: invert
    // **Apply Inverse Color Effect**
    color = vec3(1.0) - color;

    FragColor = vec4(color, 1.0);
}