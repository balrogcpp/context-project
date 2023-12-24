// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldViewProjMatrix;

in highp vec3 vertex;
out highp vec3 vUV0;
void main()
{
    vUV0 = normalize(vertex);
    gl_Position = mul(WorldViewProjMatrix, vec4(vertex, 1.0));
}
