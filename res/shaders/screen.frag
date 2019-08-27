#version 330 core
uniform sampler2D tex0;
uniform float time;
in vec2 uv;
out vec4 outColor;
float clamp01(float v)
{return clamp(v,0,1);}
vec2 get_normal(vec2 dir)
{
    return vec2(dir.y, -dir.x);
}
struct Dir
{
    vec2 dir, normal;
};
Dir setDir(vec2 d)
{
    vec2 n = normalize(d);
    return Dir(n, get_normal(n));
}
struct WallInfo
{
    vec2 pointLeft, pointRight;
    Dir innerLeft, innerRight;
    Dir outerLeft, outerRight;
    Dir dir;
};
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
    wi.dir=setDir(wi.pointRight-wi.pointLeft);
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
    const float spacing = 0.25;
    float top = 0.6+(cos(time*0.5)*0.5+0.5)*0.4;
    vec4 wall[2]= vec4[2](vec4(0.2,top, 0.45,top), vec4(0.55,top, 0.8,top));
    const vec2 posLights[3]=vec2[3](vec2(0.5-spacing, 0.5), vec2(0.5, 0.5), vec2(0.5+spacing, 0.5));
    const vec3 colLights[3]=vec3[3](vec3(1,0,0)*1.5, vec3(0,1,0), vec3(0,0,1));
    float sizeLights=0.1;
    float lengthLights=0.5;
    vec3 finalColor=vec3(0);
    

    for(int iLight=0;iLight<3;++iLight)
    {
        float valwhite=1;
        for(int iWall=0;iWall<2;++iWall)
        {
            WallInfo winfo = setWallInfo(wall[iWall], posLights[iLight], sizeLights);
            
            if (distance(posLights[iLight], uv)<=sizeLights)
                valwhite=0.0;
            else if (dot((uv-wall[iWall].xy), winfo.dir.normal)>0);
            else if (dot(winfo.innerLeft.normal, winfo.pointLeft - uv)>0 && dot(winfo.innerRight.normal, winfo.pointRight - uv)<0)
                valwhite=0.;
            else if (sizeLights>0.)
            {
                if (dot(winfo.innerLeft.normal, winfo.pointLeft - uv)<0 && dot(winfo.innerRight.normal, winfo.pointRight - uv)>0)
                {
                    // InnerLeft et InnerRight se croisent, s'en occuper plus tard
                }
                else 
                {
                    if (dot(winfo.innerLeft.normal, winfo.pointLeft - uv)<0 && dot(winfo.outerLeft.normal, winfo.pointLeft - uv)>0)
                        valwhite-=1-smoothstep (dot(winfo.innerLeft.dir, winfo.outerLeft.dir), 1, dot(normalize(winfo.pointLeft - uv), winfo.outerLeft.dir));
                    if (dot(winfo.innerRight.normal, winfo.pointRight - uv)>0 && dot(winfo.outerRight.normal, winfo.pointRight - uv)<0)
                        valwhite-=1-smoothstep (dot(winfo.innerRight.dir, winfo.outerRight.dir), 1, dot(normalize(winfo.pointRight - uv), winfo.outerRight.dir));
                }
                
            }
        }
        finalColor+=colLights[iLight]*clamp01(valwhite)*clamp01((lengthLights-distance(posLights[iLight], uv)+sizeLights)/lengthLights);
    }

    
    outColor=vec4(clamp(finalColor, 0, 1), 1.0);
}