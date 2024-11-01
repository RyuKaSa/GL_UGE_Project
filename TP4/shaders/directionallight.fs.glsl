#version 330 core

in vec3 vNormal;
in vec3 vFragPos;
in vec2 vTexCoords;
in vec4 vFragPosLightSpace; // New: Fragment position in light space

out vec4 FragColor;

// Material properties
uniform vec3 uKd;         // Diffuse reflection coefficient
uniform vec3 uKs;         // Specular reflection coefficient
uniform float uShininess; // Shininess exponent

// Light properties
uniform vec3 uLightDir_vs;      // Light direction in view space
uniform vec3 uLightIntensity;   // Light intensity (color)

// Texture sampler
uniform sampler2D uTexture;
uniform float uUseTexture; // Use 0.0 for false, 1.0 for true

// Shadow map
uniform sampler2D shadowMap;

// Function to calculate Blinn-Phong lighting
vec3 blinnPhong(vec3 Kd) {
    vec3 N = normalize(vNormal);
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

// Function to calculate shadow factor
float calculateShadow(vec4 fragPosLightSpace)
{
    // Perform perspective divide to get normalized light space coordinates
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5; // Transform to [0,1] range for texture coordinates

    // Sample the closest depth from the shadow map (from light's perspective)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;

    // Add a small bias to prevent shadow acne
    float bias = max(0.05 * (1.0 - dot(vNormal, normalize(-uLightDir_vs))), 0.005);
    
    float shadow = currentDepth - bias > closestDepth ? 0.0 : 1.0;

    return shadow;
}

void main()
{
    // Determine material color (either from texture or uniform)
    vec3 Kd = (uUseTexture > 0.5) ? texture(uTexture, vTexCoords).rgb : uKd;

    // Calculate the Blinn-Phong lighting model result
    vec3 color = blinnPhong(Kd);

    // Define an ambient light component based on material color
    vec3 ambientLight = vec3(0.1);
    vec3 ambient = ambientLight * Kd;

    // Calculate shadow factor using light space position
    float shadow = calculateShadow(vFragPosLightSpace);

    // Combine Blinn-Phong color, ambient, and shadow factor
    color = (ambient + color) * shadow;

    FragColor = vec4(color, 1.0);
}