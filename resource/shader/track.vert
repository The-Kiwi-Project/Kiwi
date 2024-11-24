#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in mat4 inInstanceMatrix;

uniform mat4 bias;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 glbal_Pos = bias * inInstanceMatrix * vec4(inPos, 1.0);
    gl_Position = projection * view * glbal_Pos;
}
