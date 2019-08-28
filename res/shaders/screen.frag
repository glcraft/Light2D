#version 330 core

#define NBLIGHTS 3
#define NBWALLS 2

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
    vec2 position;
    vec3 color;
    float size, strenght;
};

uniform float time;
in WallInfo vs_walls[NBLIGHTS*NBWALLS];
in vec2 uv;
in Light lights[NBLIGHTS];

out vec4 outColor;

float clamp01(float v)
{return clamp(v,0,1);}


void main()
{
    const float spacing = 0.25;
    vec3 finalColor=vec3(0);

    for(int iLight=0;iLight<3;++iLight)
    {
        float valwhite=1;
        for(int iWall=0;iWall<2;++iWall)
        {
            int idWall = iLight*NBWALLS+iWall;
            if (distance(lights[iLight].position, uv)<=lights[iLight].size)
                valwhite=0.0;
            else if (dot((uv-vs_walls[idWall].pointLeft), vs_walls[idWall].direction.normal)>0);
            else if (dot(vs_walls[idWall].innerLeft.normal, vs_walls[idWall].pointLeft - uv)>0 && dot(vs_walls[idWall].innerRight.normal, vs_walls[idWall].pointRight - uv)<0)
                valwhite=0.;
            else if (lights[iLight].size>0.)
            {
                if (dot(vs_walls[idWall].innerLeft.normal, vs_walls[idWall].pointLeft - uv)<0 && dot(vs_walls[idWall].innerRight.normal, vs_walls[idWall].pointRight - uv)>0)
                {
                    // InnerLeft et InnerRight se croisent, s'en occuper plus tard
                }
                else 
                {
                    if (dot(vs_walls[idWall].innerLeft.normal, vs_walls[idWall].pointLeft - uv)<0 && dot(vs_walls[idWall].outerLeft.normal, vs_walls[idWall].pointLeft - uv)>0)
                        valwhite-=1-smoothstep (dot(vs_walls[idWall].innerLeft.line, vs_walls[idWall].outerLeft.line), 1, dot(normalize(vs_walls[idWall].pointLeft - uv), vs_walls[idWall].outerLeft.line));
                    if (dot(vs_walls[idWall].innerRight.normal, vs_walls[idWall].pointRight - uv)>0 && dot(vs_walls[idWall].outerRight.normal, vs_walls[idWall].pointRight - uv)<0)
                        valwhite-=1-smoothstep (dot(vs_walls[idWall].innerRight.line, vs_walls[idWall].outerRight.line), 1, dot(normalize(vs_walls[idWall].pointRight - uv), vs_walls[idWall].outerRight.line));
                }
                
            }
        }
        finalColor+=lights[iLight].color*clamp01(valwhite)*clamp01((lights[iLight].strenght-distance(lights[iLight].position, uv)+lights[iLight].size)/lights[iLight].strenght);
    }

    
    outColor=vec4(clamp(finalColor, 0, 1), 1.0);
}