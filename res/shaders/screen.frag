#version 330 core

#define MAX_NUM_TOTAL_LIGHTS 10
#define MAX_NUM_TOTAL_WALLS 10
#define DEBUG 1

struct Dir
{
    vec2 line;
    vec2 normal;
};
struct Wall
{
    vec2 pointLeft, pointRight;
    Dir direction;
};
struct WallTangent
{
    Dir innerLeft, innerRight;
    Dir outerLeft, outerRight;
};
struct Light
{
    vec2 position, size_strength;
    vec4 color;
};

layout (std140) uniform Lights
{
    Light lights[MAX_NUM_TOTAL_LIGHTS];
    int nbLights;
};
layout (std140) uniform Walls
{
    Wall walls[MAX_NUM_TOTAL_WALLS];
    WallTangent walltangs[MAX_NUM_TOTAL_WALLS*MAX_NUM_TOTAL_LIGHTS];
    int nbWalls;
};
float cross(vec2 v, vec2 w)
{
    return v.x*w.y-v.y*w.x;
}

uniform float time;
in vec2 uv;

out vec4 outColor;

float clamp01(float v)
{return clamp(v,0,1);}


void main()
{
    vec3 finalColor=vec3(0);

    for(int iLight=0;iLight<nbLights;++iLight)
    {
        float valwhite=1;
        float distLightUV = distance(lights[iLight].position, uv);
        float size_and_strength = lights[iLight].size_strength.y+lights[iLight].size_strength.x;

        for(int iWall=0;iWall<nbWalls;++iWall)
        {
            int idWall = iLight*nbWalls+iWall;
#if DEBUG
            if (distLightUV<=lights[iLight].size_strength.x)//
                valwhite=0.0;
            else
#endif
            if (distLightUV>size_and_strength)//
                valwhite=0.0;
            else if (dot((uv-walls[iWall].pointLeft), walls[iWall].direction.normal)>0);
            else if (dot(walltangs[idWall].innerLeft.normal, walls[iWall].pointLeft - uv)>0 && dot(walltangs[idWall].innerRight.normal, walls[iWall].pointRight - uv)<0)
            {
                valwhite=0.;
            }
                
            else if (lights[iLight].size_strength.x>0.)
            {
                if (dot(walltangs[idWall].innerLeft.normal, walls[iWall].pointLeft - uv)<0 && dot(walltangs[idWall].innerRight.normal, walls[iWall].pointRight - uv)>0)
                {
                    // InnerLeft et InnerRight se croisent, s'en occuper plus tard
                    // vector finding cross point : https://stackoverflow.com/a/565282/6345054
                    float v1=smoothstep (dot(walltangs[idWall].innerLeft.line, walltangs[idWall].outerLeft.line), 1, dot(normalize(walls[iWall].pointLeft - uv), walltangs[idWall].outerLeft.line));
                    float v2=smoothstep (dot(walltangs[idWall].innerRight.line, walltangs[idWall].outerRight.line), 1, dot(normalize(walls[iWall].pointRight - uv), walltangs[idWall].outerRight.line));
                    vec2 p = walls[iWall].pointLeft, r=walltangs[idWall].innerLeft.line, q=walls[iWall].pointRight, s=walltangs[idWall].innerRight.line;
                    float t=cross((q-p), s/(cross(r, s)));
                    vec2 pt = walls[iWall].pointLeft+t*walltangs[idWall].innerLeft.line;
                    float valPT=mix(v2, v1, smoothstep (dot(walltangs[idWall].innerLeft.line, walltangs[idWall].innerRight.line), 1, dot(normalize(pt - uv), walltangs[idWall].innerRight.line)));
                    valwhite=valPT;
                }
                else 
                {
                    if (dot(walltangs[idWall].innerLeft.normal, walls[iWall].pointLeft - uv)<0 && dot(walltangs[idWall].outerLeft.normal, walls[iWall].pointLeft - uv)>0)
                        valwhite-=1-smoothstep (dot(walltangs[idWall].innerLeft.line, walltangs[idWall].outerLeft.line), 1, dot(normalize(walls[iWall].pointLeft - uv), walltangs[idWall].outerLeft.line));
                    if (dot(walltangs[idWall].innerRight.normal, walls[iWall].pointRight - uv)>0 && dot(walltangs[idWall].outerRight.normal, walls[iWall].pointRight - uv)<0)
                        valwhite-=1-smoothstep (dot(walltangs[idWall].innerRight.line, walltangs[idWall].outerRight.line), 1, dot(normalize(walls[iWall].pointRight - uv), walltangs[idWall].outerRight.line));
                }
                
            }
        }
        if (valwhite>0.)
            finalColor+=lights[iLight].color.xyz*clamp01(valwhite)*clamp01((size_and_strength-distLightUV)/lights[iLight].size_strength.y);
    }

    
    outColor=vec4(clamp(finalColor, 0, 1), 1.0);
    // outColor=vec4(vec3(0,nbWalls==2,nbLights==1), 1.0);
}