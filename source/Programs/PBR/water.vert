// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldMatrix;
uniform highp mat4 WorldViewProjMatrix;

in highp vec4 position;
in highp vec2 uv0;
out highp vec3 vPosition;
out highp vec2 vUV0;
void main()
{
    highp vec4 world = mulMat4x4Float3(WorldMatrix, position.xyz);
    vPosition = world.xyz / world.w;

    vUV0 = uv0.xy;
    gl_Position = mulMat4x4Float3(WorldViewProjMatrix, position.xyz);
}
