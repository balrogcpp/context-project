// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldViewProj;

in highp vec3 vertex;
in highp vec3 normal;
out highp vec3 vRay;
void main()
{
    gl_Position = mul(WorldViewProj, vec4(vertex, 1.0));
    vRay = normal.xyz;
}
