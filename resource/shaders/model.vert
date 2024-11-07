#version 330 core

layout (location = 0) in vec3 inPos;

out vec4 pos;

uniform mat4 bias;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 glbal_Pos = bias  * vec4(inPos, 1.0);
    gl_Position = projection * view * glbal_Pos;
    pos = glbal_Pos;
}
