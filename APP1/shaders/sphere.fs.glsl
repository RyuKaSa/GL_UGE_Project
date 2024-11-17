#version 330 core

// Inputs from the vertex shader
in vec3 vPosition_vs;   // Position in view space
in vec3 vNormal_vs;     // Normal in view space
in vec2 vTexCoords;     // Texture coordinates

// Output color
out vec4 FragColor;

void main() {
    // Normalize the normal vector
    vec3 normal = normalize(vNormal_vs);
    
    // Use the normalized normal vector as the fragment color (for debugging purposes)
    FragColor = vec4(normal * 0.5 + 0.5, 1.0); // Shift from [-1, 1] to [0, 1]
}