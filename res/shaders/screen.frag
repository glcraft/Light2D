#version 330 core

#define PI 3.1415926535
#define HALF_PI 1.5707963267

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
    ivec4 sens;
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
float det(vec2 v, vec2 w)
{
    return v.x*w.y-v.y*w.x;
}

uniform float time;
in vec2 uv;

out vec4 outColor;

float clamp01(float v)
{return clamp(v,0,1);}

float COStoLIN(float val)
{ return acos(val) / PI; }

float smooth_vectors(vec2 inner, vec2 outer, vec2 varia)
{
    return smoothstep (COStoLIN(dot(inner, outer)), 0, COStoLIN(dot(varia, outer)));
}


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
            vec2 wall_normal, wall_pointLeft, wall_pointRight;
            if(walltangs[idWall].sens.x==1)
            {
                wall_normal = -walls[iWall].direction.normal;
                wall_pointLeft = walls[iWall].pointRight;
                wall_pointRight = walls[iWall].pointLeft;
            }
            else
            {
                wall_normal = walls[iWall].direction.normal;
                wall_pointLeft = walls[iWall].pointLeft;
                wall_pointRight = walls[iWall].pointRight;
            }
#if DEBUG
            if (distLightUV<=lights[iLight].size_strength.x)//
                valwhite=0.0;
            else
#endif
            // Le fragment dépasse la limite de la source lumineuse
            if (distLightUV>size_and_strength)
                newValWhite=0.0;
            // else if (dot(walltangs[idWall].outerLeft, wall_normal)<0 || dot(walltangs[idWall].outerRight, wall_normal)<0) continue;
            // La fragment est entre la lumière et le mur
            else if (dot((uv-wall_pointLeft), wall_normal)>0);
            // Le fragment est derrière le mur par rapport à la lumière
            else if (dot(get_normal(walltangs[idWall].innerLeft), wall_pointLeft - uv)>0 && dot(get_normal(walltangs[idWall].innerRight), wall_pointRight - uv)<0)
            {
                newValWhite=0.;
            }
            // Si la taille de la lumière n'est pas nulle.
            else if (lights[iLight].size_strength.x>0.)
            {
                
                // Le fragment se situe après le croisement des inner (dans le cas ou la source lumineuse est plus grosse que le mur)
                if (dot(get_normal(walltangs[idWall].innerLeft), wall_pointLeft - uv)<0 && dot(get_normal(walltangs[idWall].innerRight), wall_pointRight - uv)>0)
                {
                    // InnerLeft et InnerRight se croisent, s'en occuper plus tard
                    // vector finding cross point : https://stackoverflow.com/a/565282/6345054
                    float v1=smooth_vectors(walltangs[idWall].innerLeft, walltangs[idWall].outerLeft, normalize(wall_pointLeft - uv));
                    float v2=smooth_vectors(walltangs[idWall].innerRight, walltangs[idWall].outerRight, normalize(wall_pointRight - uv));
                    vec2 p = wall_pointLeft, r=walltangs[idWall].innerLeft, q=wall_pointRight, s=walltangs[idWall].innerRight;
                    float t=det((q-p), s/(det(r, s)));
                    vec2 pt = wall_pointLeft+t*walltangs[idWall].innerLeft;
                    float valPT = mix(v2, v1, smooth_vectors(walltangs[idWall].innerLeft, walltangs[idWall].innerRight, normalize(pt - uv)));
                    newValWhite=valPT;
                }
                else 
                {
                    // Le fragment est dans la transition gauche
                    if (dot(get_normal(walltangs[idWall].innerLeft), wall_pointLeft - uv)<0 && dot(get_normal(walltangs[idWall].outerLeft), wall_pointLeft - uv)>0)
                    {
                        newValWhite=smooth_vectors(walltangs[idWall].innerLeft, walltangs[idWall].outerLeft, normalize(wall_pointLeft - uv));
                    }
                    // Le fragment est dans la transition droite
                    if (dot(get_normal(walltangs[idWall].innerRight), wall_pointRight - uv)>0 && dot(get_normal(walltangs[idWall].outerRight), wall_pointRight - uv)<0)
                    {
                        newValWhite=smooth_vectors(walltangs[idWall].innerRight, walltangs[idWall].outerRight, normalize(wall_pointRight - uv));
                    }
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