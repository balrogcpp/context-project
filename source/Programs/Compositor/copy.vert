// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldViewProj;

in highp vec4 vertex;
void main()
{
    gl_Position = mul(WorldViewProj, vertex);
}
