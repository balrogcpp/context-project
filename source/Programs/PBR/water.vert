// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldMatrix;
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;

in highp vec3 vertex;
out highp vec3 vPosition;
void main()
{
    highp vec4 world = mul(WorldMatrix, vec4(vertex, 1.0));
    vPosition = world.xyz / world.w;

    gl_Position = mul(WorldViewProjMatrix, vec4(vertex, 1.0));
}
