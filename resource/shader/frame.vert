#version 330 core
layout (location = 0) in vec3 inPos;

uniform mat4 bias;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * bias * vec4(inPos, 1.0);
}
