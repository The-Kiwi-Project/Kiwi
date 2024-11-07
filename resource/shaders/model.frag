#version 330 core

in vec4 pos;

out vec4 outColor;

void main()
{
    // vec3 color = pos.xyz + vec3(5, 5, 5);
    outColor = vec4(1.0, 0.5, 0.5, 1.0);
}
