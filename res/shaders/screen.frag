#version 330 core
uniform sampler2D tex0;
in vec2 uv;
out vec4 outColor;
void main()
{
    outColor=texture(tex0, uv);
}