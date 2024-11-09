#version 330 core

in vec3 vNormal;
in vec3 vFragPosWorld;
in vec2 vTexCoords;
in mat3 TBN;

out vec4 FragColor;

// Material properties
uniform vec3 uKd;         // Diffuse reflection coefficient
uniform vec3 uKs;         // Specular reflection coefficient
uniform float uShininess; // Shininess exponent

// Light properties
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

float ShadowCalculation(vec3 fragPosWorld) {
    vec3 fragToLight = fragPosWorld - lightPosWorld;
    float currentDepth = length(fragToLight);
    float shadow = 0.0;
    float bias = 0.05;
    int samples = 8;
    float diskRadius = 0.1; // Adjust as needed

    // Generate random sampling offsets
    vec3 randomOffset = vec3(0.0);
    for (int i = 0; i < samples; ++i)
    {
        // Generate a random vector in the unit sphere
        vec3 offset = normalize(vec3(
            fract(sin(dot(fragPosWorld.xy, vec2(12.9898, 78.233))) * 43758.5453 + i) * 2.0 - 1.0,
            fract(sin(dot(fragPosWorld.yz, vec2(12.9898, 78.233))) * 43758.5453 + i) * 2.0 - 1.0,
            fract(sin(dot(fragPosWorld.zx, vec2(12.9898, 78.233))) * 43758.5453 + i) * 2.0 - 1.0
        ));
        float closestDepth = texture(depthMap, fragToLight + offset * diskRadius).r;
        closestDepth *= farPlane;
        if (currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);

    return shadow;
}

vec3 blinnPhong(vec3 Kd, vec3 N) {
    vec3 L = normalize(lightPosWorld - vFragPosWorld); // Light direction vector from light to fragment
    vec3 V = normalize(cameraPosWorld - vFragPosWorld); // View direction vector from fragment to camera
    vec3 H = normalize(L + V); // Halfway vector

    // Correct the attenuation calculation
    float distanceToLight = length(lightPosWorld - vFragPosWorld);

    // Attenuation factors (adjust as necessary)
    float constant = 1.0;
    float linear = 0.14;
    float quadratic = 0.07;
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
    color = ambient + (1.0 - shadow) * color;

    FragColor = vec4(color, 1.0);
}