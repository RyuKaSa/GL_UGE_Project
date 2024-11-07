#version 330 core

in vec3 vNormal;
in vec3 vFragPos;
in vec2 vTexCoords;
in vec4 vFragPosLightSpace;
in mat3 TBN;

out vec4 FragColor;

// Material properties
uniform vec3 uKd;         // Diffuse reflection coefficient
uniform vec3 uKs;         // Specular reflection coefficient
uniform float uShininess; // Shininess exponent

// Light properties
uniform vec3 uLightDir_vs;    // Light direction in view space
uniform vec3 uLightIntensity; // Light intensity (color)

// Texture sampler
uniform sampler2D uTexture;
uniform float uUseTexture; // Use 0.0 for false, 1.0 for true

// Shadow map
uniform sampler2D shadowMap;

// Normal map
uniform sampler2D uNormalMap;
uniform float uUseNormalMap; // Use 0.0 for false, 1.0 for true

// Function to calculate Blinn-Phong lighting
vec3 blinnPhong(vec3 Kd, vec3 N) {
    vec3 L = normalize(-uLightDir_vs);
    vec3 V = normalize(-vFragPos);
    vec3 H = normalize(L + V);

    // Apply dynamic light color to diffuse component
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = Kd * uLightIntensity * NdotL;

    // Apply dynamic light color to specular component
    float NdotH = max(dot(N, H), 0.0);
    vec3 specular = uKs * uLightIntensity * pow(NdotH, uShininess);

    // Final color
    return diffuse + specular;
}

// Function to calculate shadow factor with Percentage-Closer Filtering (PCF)
float calculateShadowPCF(vec4 fragPosLightSpace) {
    // Perform perspective divide to get normalized device coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range for texture coordinates
    projCoords = projCoords * 0.5 + 0.5;

    // Check if fragment is outside the light's orthographic frustum
    if (projCoords.z > 1.0)
        return 1.0;

    // Add a small bias to prevent shadow acne
    float bias = max(0.005 * (1.0 - dot(vNormal, normalize(-uLightDir_vs))), 0.0005);

    // PCF parameters
    float shadow = 0.0;
    float texelSize = 1.0 / 8192.0; // Assuming a shadow map resolution of 8192x8192

    // PCF loop for a 3x3 sample grid
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += (projCoords.z - bias > pcfDepth) ? 0.0 : 1.0;
        }
    }
    shadow /= 9.0; // Average the shadow factor

    return shadow;
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

    // Calculate shadow factor using PCF
    float shadow = calculateShadowPCF(vFragPosLightSpace);

    // Combine Blinn-Phong color, ambient, and shadow factor
    color = ambient + (color * shadow);

    FragColor = vec4(color, 1.0);
}