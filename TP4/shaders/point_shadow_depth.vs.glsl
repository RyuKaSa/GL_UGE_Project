#version 330 core
layout(location = 0) in vec3 aPosition;

uniform mat4 model;

out vec4 FragPosWorld;

void main()
{
    FragPosWorld = model * vec4(aPosition, 1.0);
}