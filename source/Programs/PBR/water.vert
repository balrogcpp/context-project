// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldMatrix;
uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;

in highp vec3 vertex;
in highp vec2 uv0;
out highp vec3 vPosition;
out highp vec2 vUV0;
void main()
{
    highp vec4 position = vec4(vertex, 1.0);
    highp vec4 world = mul(WorldMatrix, position);

    vPosition = world.xyz / world.w;

    vUV0 = uv0;
    gl_Position = mul(WorldViewProjMatrix, position);
}
