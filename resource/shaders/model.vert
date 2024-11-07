#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 biasPos;

uniform mat4 bias;
uniform mat4 view;
uniform mat4 projection;

out vec2 texCoord;

void main()
{
    vec3 pos = inPos.xyz + biasPos.xyz;
    gl_Position = projection * view * bias * vec4(pos, 1.0);
    texCoord = inCoord;
}
