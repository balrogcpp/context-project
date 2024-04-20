// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldViewProj;

in highp vec3 vertex;

void main()
{
    gl_Position = mul(WorldViewProj, vec4(vertex, 1.0));
}
