// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldViewProjMatrix;

in highp vec4 vertex;
out highp vec3 vUV0;
void main()
{
    vUV0 = normalize(vertex.xyz);
    gl_Position = mul(WorldViewProjMatrix, vertex);
}
