#version 330 core
layout (location = 0) in vec3 aPos; // Vertex position
layout (location = 1) in vec3 aNormal; // Vertex normal
layout (location = 2) in vec2 aTexCoords; // Texture coordinates

out vec3 FragPos; // Output world-space position
out vec3 Normal; // Output world-space normal
out vec2 TexCoords; // Output texture coordinates

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Transform position to world space
    FragPos = vec3(model * vec4(aPos, 1.0));
    // Transform normal to world space
    Normal = mat3(transpose(inverse(model))) * aNormal;
    // Pass texture coordinates
    TexCoords = aTexCoords;

    // Transform position to clip space
    gl_Position = projection * view * vec4(FragPos, 1.0);
}