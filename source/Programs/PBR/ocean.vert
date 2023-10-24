// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldMatrix;
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;
uniform highp mat4 TextureProjMatrix;

in highp vec4 vertex;
out highp vec3 vWorldPosition;
out vec4 vScreenPosition;
out vec4 vProjectionCoord;
void main()
{
    highp vec4 world = mul(WorldMatrix, vertex);
    vWorldPosition = world.xyz / world.w;
    vProjectionCoord = mul(TextureProjMatrix, vertex);

    gl_Position = mul(WorldViewProjMatrix, vertex);
    vScreenPosition = gl_Position;
}
