#version 330 core
uniform sampler2D tex0;
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
    wi.dir=setDir(wall.zw-wall.xy);
    if (size>0)
    {
        vec4 tan1 = finding_tangent(posLight, size, wall.xy), tan2 = finding_tangent(posLight, size, wall.zw);
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
    const float top = 0.6;
    const vec2 pts[2]= vec2[2](vec2(0.4,top), vec2(0.6,top));
    const vec2 posPlayer=vec2(0.4, 0.5);
    float sizePlayer=0.05;

    WallInfo winfo = setWallInfo(vec4(pts[0], pts[1]), posPlayer, sizePlayer);
    float valwhite=1;
    if (dot((uv-pts[1]), winfo.dir.normal)>0);
    else if (dot(winfo.innerLeft.normal, winfo.pointLeft - uv)>0 && dot(winfo.innerRight.normal, winfo.pointRight - uv)<0)
        valwhite=0.;
    else if (sizePlayer>0.)
    {
        if (dot(winfo.innerLeft.normal, winfo.pointLeft - uv)<0 && dot(winfo.outerLeft.normal, winfo.pointLeft - uv)>0)
            valwhite=smoothstep (dot(winfo.innerLeft.dir, winfo.outerLeft.dir), 1, dot(normalize(winfo.pointLeft - uv), winfo.outerLeft.dir));
        else if (dot(winfo.innerRight.normal, winfo.pointRight - uv)>0 && dot(winfo.outerRight.normal, winfo.pointRight - uv)<0)
            valwhite=smoothstep (dot(winfo.innerRight.dir, winfo.outerRight.dir), 1, dot(normalize(winfo.pointRight - uv), winfo.outerRight.dir));
    }
    outColor=vec4(vec3(clamp01(valwhite)), 1.0);
}