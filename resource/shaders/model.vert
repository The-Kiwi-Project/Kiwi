#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 biasPos;

uniform mat4 bias;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec3 pos = inPos.xyz + biasPos.xyz;
    gl_Position = projection * view * bias * vec4(pos, 1.0);
}
