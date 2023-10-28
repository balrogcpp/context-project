// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldViewProjMatrix;
uniform highp mat4 WorldViewProjPrev;

in highp vec4 vertex;
in vec3 uv0;
out vec3 vUV0;
void main()
{
    vUV0 = uv0;
    gl_Position = mul(WorldViewProjMatrix, vertex);
}
