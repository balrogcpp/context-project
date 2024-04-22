// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldViewProj;

in highp vec3 vertex;

out highp vec2 vUV0;
void main()
{
    gl_Position = mul(WorldViewProj, vec4(vertex, 1.0));
    vec2 inPos = sign(vertex.xy);
    vUV0 = vec2(inPos.x, -inPos.y) * 0.5 + 0.5;
}
