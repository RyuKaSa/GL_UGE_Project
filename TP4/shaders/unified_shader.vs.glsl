#version 330 core

layout(location = 0) in vec3 aPosition;     // Vertex position
layout(location = 1) in vec3 aNormal;       // Vertex normal
layout(location = 2) in vec2 aTexCoords;    // Texture coordinates
layout(location = 3) in vec3 aTangent;      // Tangent vector
layout(location = 4) in vec3 aBitangent;    // Bitangent vector

uniform mat4 uMVPMatrix;
uniform mat4 uMVMatrix;
uniform mat3 uNormalMatrix;
uniform mat4 lightSpaceMatrix;
uniform mat4 uModelMatrix;

out vec3 vNormal;
out vec3 vFragPos;
out vec2 vTexCoords;
out vec4 vFragPosLightSpace;
out mat3 TBN; // Pass the TBN matrix to the fragment shader

void main()
{
    vNormal = normalize(uNormalMatrix * aNormal);
    vFragPos = vec3(uMVMatrix * vec4(aPosition, 1.0));
    vTexCoords = aTexCoords;

    // Calculate TBN matrix
    vec3 T = normalize(vec3(uModelMatrix * vec4(aTangent, 0.0)));
    vec3 B = normalize(vec3(uModelMatrix * vec4(aBitangent, 0.0)));
    vec3 N = normalize(vec3(uModelMatrix * vec4(aNormal, 0.0)));
    TBN = mat3(T, B, N);

    // Transform fragment position to light space
    vFragPosLightSpace = lightSpaceMatrix * uModelMatrix * vec4(aPosition, 1.0);

    gl_Position = uMVPMatrix * vec4(aPosition, 1.0);
}