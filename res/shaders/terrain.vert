#version 330 core
layout (location=0) in vec2 pos;
uniform mat4 projmat, viewmat;
void main()
{
    gl_Position = projmat * viewmat * vec4(pos, 0, 1);
}