// created by Andrey Vasiliev

#include "header.glsl"

uniform highp mat4 WorldViewProjMatrix;

in highp vec3 vertex;
in vec3 uv0;
out vec3 vUV0;
void main()
{
    vUV0 = uv0;
    vUV0.z = -vUV0.z;
    gl_Position = mul(WorldViewProjMatrix, vec4(vertex, 1.0));
}
