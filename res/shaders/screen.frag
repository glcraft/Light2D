#version 330 core
uniform sampler2D tex0;
in vec2 uv;
out vec4 outColor;
float clamp01(float v)
{return clamp(v,0,1);}
void main()
{
    const float valstep = 0.001;
    const vec2 posPlayer = vec2(0.5);
    vec2 vecstep = normalize(uv-posPlayer)*valstep;
    vec2 currentUV = uv;
    float currentVal = 0;
    int i=0;
    while(dot(vecstep, currentUV-posPlayer)>0)
        currentVal+=texture(tex0, currentUV).x*0.05, currentUV-=vecstep;
    outColor=vec4(vec3(0), clamp01(currentVal));
}