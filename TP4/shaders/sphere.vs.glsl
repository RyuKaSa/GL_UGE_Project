#version 330 core

// Input attributes from the VBO
layout(location = 0) in vec3 aVertexPosition; // Vertex position
layout(location = 1) in vec3 aVertexNormal;   // Vertex normal
layout(location = 2) in vec2 aVertexTexCoords; // Vertex texture coordinates

// Uniforms for transformation matrices
uniform mat4 uMVPMatrix;     // Model-View-Projection matrix
uniform mat4 uMVMatrix;      // Model-View matrix
uniform mat4 uNormalMatrix;  // Normal matrix

// Outputs to the fragment shader
out vec3 vPosition_vs;       // Vertex position in view space
out vec3 vNormal_vs;         // Normal vector in view space
out vec2 vTexCoords;         // Texture coordinates

void main() {
    // Transform the vertex position to view space and pass to fragment shader
    vPosition_vs = vec3(uMVMatrix * vec4(aVertexPosition, 1.0));

    // Transform the normal vector to view space and pass to fragment shader
    vNormal_vs = vec3(uNormalMatrix * vec4(aVertexNormal, 0.0));

    // Pass texture coordinates directly to the fragment shader
    vTexCoords = aVertexTexCoords;

    // Compute the final position of the vertex in clip space
    gl_Position = uMVPMatrix * vec4(aVertexPosition, 1.0);
}