// created by Andrey Vasiliev

#include "header.glsl"

uniform mat4 WorldViewProj;

in highp vec3 vertex;
in highp vec3 normal;

out vec2 vUV0;
out vec3 vRay;
void main()
{
    gl_Position = mul(WorldViewProj, vec4(vertex, 1.0));
    vec2 inPos = sign(vertex.xy);
    vUV0 = vec2(inPos.x, -inPos.y) * 0.5 + 0.5;
    vRay = normal.xyz;
}
