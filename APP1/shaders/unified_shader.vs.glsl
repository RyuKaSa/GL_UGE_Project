#version 330 core

layout(location = 0) in vec3 aPosition;     // Vertex position
layout(location = 1) in vec3 aNormal;       // Vertex normal
layout(location = 2) in vec2 aTexCoords;    // Texture coordinates
layout(location = 3) in vec3 aTangent;      // Tangent vector
layout(location = 4) in vec3 aBitangent;    // Bitangent vector

// Uniforms
uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat3 uNormalMatrix;
uniform mat4 lightSpaceMatrix;
uniform mat4 uModelMatrix;

out vec3 vNormal;
out vec3 vFragPos;
out vec2 vTexCoords;
out vec4 vFragPosLightSpace;
out mat3 TBN; // Tangent-Bitangent-Normal matrix
out vec3 vFragPosWorld;

void main()
{
    vNormal = normalize(uNormalMatrix * aNormal);
    vFragPos = vec3(uMVMatrix * vec4(aPosition, 1.0));
    vTexCoords = aTexCoords;
    
    // Calculate world space fragment position
    vFragPosWorld = vec3(uModelMatrix * vec4(aPosition, 1.0));

    // Transform TBN vectors into view space
    vec3 T = normalize(uNormalMatrix * aTangent);
    vec3 B = normalize(uNormalMatrix * aBitangent);
    vec3 N = normalize(uNormalMatrix * aNormal);
    TBN = mat3(T, B, N);

    // Transform fragment position to light space
    vFragPosLightSpace = lightSpaceMatrix * uModelMatrix * vec4(aPosition, 1.0);

    gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
}