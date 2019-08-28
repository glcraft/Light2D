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
    Dir direction;
    vec2 pointLeft, pointRight;
    Dir innerLeft, innerRight;
    Dir outerLeft, outerRight;
    
};
struct Light
{
    vec2 position;
    vec3 color;
    float size, strenght;
};

uniform float time;

layout (location=0) in vec2 pos;

out WallInfo vs_walls[NBLIGHTS*NBWALLS];
out vec2 uv;
out Light lights[NBLIGHTS];


vec2 get_normal(vec2 dir)
{return vec2(dir.y, -dir.x);}

Dir setDir(vec2 d)
{
    vec2 n = normalize(d);
    return Dir(n, get_normal(n));
}

// https://stackoverflow.com/a/15846131/6345054
vec4 finding_tangent(vec2 center_circle, float radius, vec2 point)
{
    vec2 d = center_circle - point;
    float dd = length(d);
    float a = asin(radius / dd);
    float b = atan(d.y, d.x);
    
    float ta = b - a, tb = b + a;
    vec4 pts=radius * vec4(sin(ta), -cos(ta), -sin(tb), cos(tb));
    return vec4((center_circle+pts.xy-point), (center_circle+pts.zw-point));
}
WallInfo setWallInfo(vec4 wall, vec2 posLight, float size)
{
    WallInfo wi;
    wi.pointLeft = wall.xy;
    wi.pointRight = wall.zw;
    wi.direction=setDir(wi.pointRight-wi.pointLeft);
    
    if (size>0)
    {
        vec4 tan1 = finding_tangent(posLight, size, wi.pointLeft), tan2 = finding_tangent(posLight, size, wi.pointRight);
        wi.innerLeft = setDir(tan1.xy);
        wi.innerRight = setDir(tan2.zw);
        wi.outerLeft = setDir(tan1.zw);
        wi.outerRight = setDir(tan2.xy);
    }
    else
    {
        wi.innerLeft = wi.outerLeft = setDir(posLight-wi.pointLeft);
        wi.innerRight = wi.outerRight = setDir(posLight-wi.pointRight);
    }
    return wi;
}

void main()
{
    gl_Position = vec4(pos*2-1, 0, 1);
    uv=pos;

    const float spacing = 0.25;
    float top = 0.6+(cos(time*0.5)*0.5+0.5)*0.4;
    vec4 wall[2]= vec4[2](vec4(0.2,top, 0.45,top), vec4(0.55,top, 0.8,top));

    const vec2 posLights[3]=vec2[3](vec2(0.5-spacing, 0.5), vec2(0.5, 0.5), vec2(0.5+spacing, 0.5));
    const vec3 colLights[3]=vec3[3](vec3(1.0, 0.0, 0.0)*1.5, vec3(0.0, 1.0, 0.0), vec3(0,0,1));
    float sizeLights=0.1;
    float lengthLights=0.5;
    
    for(int iLight=0;iLight<3;++iLight)
    {
        float valwhite=1;
        lights[iLight].position = posLights[iLight];
        lights[iLight].color = colLights[iLight];
        lights[iLight].size = sizeLights;
        lights[iLight].strenght = lengthLights;
        for(int iWall=0;iWall<2;++iWall)
        {
            vs_walls[iLight*2+iWall] = setWallInfo(wall[iWall], posLights[iLight], sizeLights);
        }
    }
}