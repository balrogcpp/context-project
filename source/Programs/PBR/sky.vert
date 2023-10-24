// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;

in highp vec4 vertex;
in highp vec4 uv0;
out highp vec3 vPosition;
out vec3 vUV0;
void main()
{
    vUV0 = uv0.xyz;
    vPosition = vertex.xyz;
    gl_Position = mul(WorldViewProjMatrix, vertex);
}
