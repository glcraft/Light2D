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
    vec2 innerLeft, innerRight;
    vec2 outerLeft, outerRight;
    
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
vec2 get_normal(vec2 dir)
{
    return vec2(dir.y, -dir.x);
}
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
            float newValWhite=1.0;
            int idWall = iLight*nbWalls+iWall;
#if DEBUG
            if (distLightUV<=lights[iLight].size_strength.x)//
                valwhite=0.0;
            else
#endif
            // Le fragment dépasse la limite de la source lumineuse
            if (distLightUV>size_and_strength)
                newValWhite=0.0;
            // else if (dot(walltangs[idWall].outerLeft, walls[iWall].direction.normal)<0 || dot(walltangs[idWall].outerRight, walls[iWall].direction.normal)<0) continue;
            // La fragment est entre la lumière et le mur
            else if (dot((uv-walls[iWall].pointLeft), walls[iWall].direction.normal)>0);
            // Le fragment est derrière le mur par rapport à la lumière
            else if (dot(get_normal(walltangs[idWall].innerLeft), walls[iWall].pointLeft - uv)>0 && dot(get_normal(walltangs[idWall].innerRight), walls[iWall].pointRight - uv)<0)
            {
                newValWhite=0.;
            }
            // Le fragment se situe à l'extérieur des inner
            else if (lights[iLight].size_strength.x>0.)
            {
                // Le fragment se situe après le croisement des inner (dans le cas ou la source lumineuse est plus grosse que le mur)
                if (dot(get_normal(walltangs[idWall].innerLeft), walls[iWall].pointLeft - uv)<0 && dot(get_normal(walltangs[idWall].innerRight), walls[iWall].pointRight - uv)>0)
                {
                    // InnerLeft et InnerRight se croisent, s'en occuper plus tard
                    // vector finding cross point : https://stackoverflow.com/a/565282/6345054
                    float v1=smoothstep (dot(walltangs[idWall].innerLeft, walltangs[idWall].outerLeft), 1, dot(normalize(walls[iWall].pointLeft - uv), walltangs[idWall].outerLeft));
                    float v2=smoothstep (dot(walltangs[idWall].innerRight, walltangs[idWall].outerRight), 1, dot(normalize(walls[iWall].pointRight - uv), walltangs[idWall].outerRight));
                    vec2 p = walls[iWall].pointLeft, r=walltangs[idWall].innerLeft, q=walls[iWall].pointRight, s=walltangs[idWall].innerRight;
                    float t=cross((q-p), s/(cross(r, s)));
                    vec2 pt = walls[iWall].pointLeft+t*walltangs[idWall].innerLeft;
                    float valPT=mix(v2, v1, smoothstep (dot(walltangs[idWall].innerLeft, walltangs[idWall].innerRight), 1, dot(normalize(pt - uv), walltangs[idWall].innerRight)));
                    newValWhite=valPT;
                }
                else 
                {
                    // Le fragment est dans la transition gauche
                    if (dot(get_normal(walltangs[idWall].innerLeft), walls[iWall].pointLeft - uv)<0 && dot(get_normal(walltangs[idWall].outerLeft), walls[iWall].pointLeft - uv)>0)
                        newValWhite=smoothstep (dot(walltangs[idWall].innerLeft, walltangs[idWall].outerLeft), 1, dot(normalize(walls[iWall].pointLeft - uv), walltangs[idWall].outerLeft));
                    // Le fragment est dans la transition droite
                    if (dot(get_normal(walltangs[idWall].innerRight), walls[iWall].pointRight - uv)>0 && dot(get_normal(walltangs[idWall].outerRight), walls[iWall].pointRight - uv)<0)
                        newValWhite=smoothstep (dot(walltangs[idWall].innerRight, walltangs[idWall].outerRight), 1, dot(normalize(walls[iWall].pointRight - uv), walltangs[idWall].outerRight));
                }
                
            }
            valwhite-=(1-newValWhite);
        }
        if (valwhite>0.)
            finalColor+=lights[iLight].color.xyz*clamp01(valwhite)*clamp01((size_and_strength-distLightUV)/lights[iLight].size_strength.y);
    }

    
    outColor=vec4(clamp(finalColor, 0, 1), 1.0);
    // outColor=vec4(vec3(0,nbWalls==2,nbLights==1), 1.0);
}