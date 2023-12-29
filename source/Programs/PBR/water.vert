// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldMatrix;
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;
uniform highp mat4 TextureProjMatrix;

in highp vec3 vertex;
out highp vec3 vPosition;
out highp vec4 vProjectionCoord;
void main()
{
    highp vec4 world = mul(WorldMatrix, vec4(vertex, 1.0));
    vPosition = world.xyz / world.w;
    vProjectionCoord = mul(TextureProjMatrix, vec4(vertex, 1.0));

    gl_Position = mul(WorldViewProjMatrix, vec4(vertex, 1.0));
}
