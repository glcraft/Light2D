#version 330 core

#define MAX_NUM_TOTAL_LIGHTS 5
#define MAX_NUM_TOTAL_WALLS 5

struct Dir
{
    vec2 line;
    vec2 normal;
};
struct WallInfo
{
    vec2 pointLeft, pointRight;
    Dir innerLeft, innerRight;
    Dir outerLeft, outerRight;
    Dir direction;
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
    WallInfo walls[MAX_NUM_TOTAL_WALLS*MAX_NUM_TOTAL_LIGHTS];
    int nbWalls;
};

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
        for(int iWall=0;iWall<nbWalls;++iWall)
        {
            int idWall = iLight*nbWalls+iWall;
            if (distance(lights[iLight].position, uv)<=lights[iLight].size_strength.x)
                valwhite=0.0;
            else if (dot((uv-walls[idWall].pointLeft), walls[idWall].direction.normal)>0);
            else if (dot(walls[idWall].innerLeft.normal, walls[idWall].pointLeft - uv)>0 && dot(walls[idWall].innerRight.normal, walls[idWall].pointRight - uv)<0)
                valwhite=0.;
            else if (lights[iLight].size_strength.x>0.)
            {
                if (dot(walls[idWall].innerLeft.normal, walls[idWall].pointLeft - uv)<0 && dot(walls[idWall].innerRight.normal, walls[idWall].pointRight - uv)>0)
                {
                    // InnerLeft et InnerRight se croisent, s'en occuper plus tard
                }
                else 
                {
                    if (dot(walls[idWall].innerLeft.normal, walls[idWall].pointLeft - uv)<0 && dot(walls[idWall].outerLeft.normal, walls[idWall].pointLeft - uv)>0)
                        valwhite-=1-smoothstep (dot(walls[idWall].innerLeft.line, walls[idWall].outerLeft.line), 1, dot(normalize(walls[idWall].pointLeft - uv), walls[idWall].outerLeft.line));
                    if (dot(walls[idWall].innerRight.normal, walls[idWall].pointRight - uv)>0 && dot(walls[idWall].outerRight.normal, walls[idWall].pointRight - uv)<0)
                        valwhite-=1-smoothstep (dot(walls[idWall].innerRight.line, walls[idWall].outerRight.line), 1, dot(normalize(walls[idWall].pointRight - uv), walls[idWall].outerRight.line));
                }
                
            }
        }
        finalColor+=lights[iLight].color.xyz*clamp01(valwhite)*clamp01((lights[iLight].size_strength.y-distance(lights[iLight].position, uv)+lights[iLight].size_strength.x)/lights[iLight].size_strength.y);
    }

    
    outColor=vec4(clamp(finalColor, 0, 1), 1.0);
    // outColor=vec4(vec3(0,nbWalls==2,nbLights==1), 1.0);
}