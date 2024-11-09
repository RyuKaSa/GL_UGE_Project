#version 330 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 shadowMatrices[6];

in vec4 FragPosWorld[];
out vec4 gFragPosWorld;

void main()
{
    for (int face = 0; face < 6; ++face)
    {
        gl_Layer = face; // Set the layer to render to the correct cube map face
        for (int i = 0; i < 3; ++i)
        {
            gFragPosWorld = FragPosWorld[i];
            gl_Position = shadowMatrices[face] * gFragPosWorld;
            EmitVertex();
        }
        EndPrimitive();
    }
}