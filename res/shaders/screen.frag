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
    
    float ta = b - a,tb = b + a;
    return vec4(radius * sin(ta), radius * -cos(ta), radius * -sin(tb), radius * cos(tb));
}
WallInfo setWallInfo(vec4 wall, vec2 posLight, float size)
{
    WallInfo wi;
    wi.dir=setDir(wall.zw-wall.xy);
    vec4 tan1 = finding_tangent(posLight, size, wall.xy), tan2 = finding_tangent(posLight, size, wall.zw);
    wi.innerLeft = setDir(tan1.xy);
    wi.innerRight = setDir(tan2.xy);
    wi.outerLeft = setDir(tan1.zw);
    wi.outerRight = setDir(tan2.zw);
    return wi;
}
void main()
{
    const float top = 0.6;
    const vec2 pts[2]= vec2[2](vec2(0.4,top), vec2(0.6,top));
    const vec2 posPlayer=vec2(0.5, 0.5);

    vec2 norm_wall=get_normal(normalize(pts[1]-pts[0]));
    // vec2 dirs[2] = vec2[2](normalize(pts[0]-posPlayer), normalize(posPlayer-pts[1]));
    vec2 dirs[2] = vec2[2](normalize(pts[0]-posPlayer), normalize(pts[1]-posPlayer));
    vec2 norms[2] = vec2[2](get_normal(dirs[0]), get_normal(dirs[1]));
    // bool iswhite = !(dot((uv-pts[0]), norms[0])>0 && dot((uv-pts[1]), norms[1])>0 && dot((uv-pts[1]), norm_wall)<0 && dot((pts[1]-posPlayer), norm_wall)<0);
    float valwhite = smoothstep (1, dot(dirs[1], dirs[0]), dot(normalize(uv-posPlayer), dirs[0]));//*float(dot((uv-pts[1]), norm_wall)<0 && dot((pts[1]-posPlayer), norm_wall)<0);

    outColor=vec4(vec3(clamp01(valwhite)), 1.0);
}