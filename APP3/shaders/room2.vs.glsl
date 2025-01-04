#version 330 core

// Vertex Attributes
layout(location = 0) in vec3 aPosition;     // Vertex position
layout(location = 1) in vec3 aNormal;       // Vertex normal
layout(location = 2) in vec2 aTexCoords;    // Texture coordinates
layout(location = 3) in vec3 aTangent;      // Tangent vector
layout(location = 4) in vec3 aBitangent;    // Bitangent vector

// Uniform Matrices
uniform mat4 uMVPMatrix;      // Model-View-Projection matrix
uniform mat4 uMVMatrix;       // Model-View matrix
uniform mat3 uNormalMatrix;   // Normal matrix

// Light Properties
uniform vec3 uLightPos_vs;    // Main light position in view space

#define MAX_ADDITIONAL_LIGHTS 8
uniform int uNumAdditionalLights;          
uniform vec3 uAdditionalLightPos[MAX_ADDITIONAL_LIGHTS]; // Additional lights in view space

// Constants for Gravitational Pull
const float GRAVITY_STRENGTH = 1.0;   // Controls intensity of gravitational pull
const float GRAVITY_RANGE = 3.0;     // Maximum range of gravitational effect
const float GRAVITY_FALLOFF = 0.1;    // Prevents division by zero

// Outputs to Fragment Shader
out vec3 vNormal;
out vec3 vFragPos;
out vec2 vTexCoords;
out vec3 vFragPosWorld;
out mat3 TBN; // Tangent-Bitangent-Normal matrix

// Pseudo-Random Function for Per-Triangle Variance
float rand(vec2 co) {
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

// Gravitational Pull Function
vec3 calculateGravitationalPull(vec3 vertexPos, vec3 lightPos, float strength, float triangleRandom) {
    vec3 toLight = normalize(lightPos - vertexPos);
    float distance = length(lightPos - vertexPos);

    if (distance < GRAVITY_RANGE) {
        float pullStrength = strength / (distance * distance + GRAVITY_FALLOFF);
        pullStrength *= triangleRandom; // Triangle-specific randomness
        return toLight * pullStrength;
    }
    return vec3(0.0);
}

void main() {
    // Calculate view-space position of the vertex
    vec3 viewPosition = (uMVMatrix * vec4(aPosition, 1.0)).xyz;
    vec3 totalDisplacement = vec3(0.0);

    // Randomness Per Triangle (using TexCoords)
    float triangleRandom = rand(aTexCoords);

    // Pull towards main light (View Space)
    totalDisplacement += calculateGravitationalPull(viewPosition, uLightPos_vs, GRAVITY_STRENGTH, triangleRandom);

    // Pull towards additional lights (View Space)
    for (int i = 0; i < uNumAdditionalLights; ++i) {
        totalDisplacement += calculateGravitationalPull(viewPosition, uAdditionalLightPos[i], GRAVITY_STRENGTH, triangleRandom);
    }

    // Apply gravitational pull displacement
    vec3 displacedPosition = aPosition + (inverse(uMVMatrix) * vec4(totalDisplacement, 0.0)).xyz;

    // Transform the displaced position
    gl_Position = uMVPMatrix * vec4(displacedPosition, 1.0);

    // Pass data to Fragment Shader
    vNormal = uNormalMatrix * aNormal;
    vFragPos = viewPosition + totalDisplacement;
    vTexCoords = aTexCoords;
    vFragPosWorld = (inverse(uMVMatrix) * vec4(viewPosition, 1.0)).xyz;

    // Construct TBN Matrix
    vec3 T = normalize(uNormalMatrix * aTangent);
    vec3 B = normalize(uNormalMatrix * aBitangent);
    vec3 N = normalize(uNormalMatrix * aNormal);
    TBN = mat3(T, B, N);
}