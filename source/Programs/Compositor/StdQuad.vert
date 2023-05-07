// created by Andrey Vasiliev

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"


attribute highp vec4 vertex;
varying mediump vec2 vUV0;
uniform highp mat4 WorldViewProj;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    gl_Position = mul(WorldViewProj, vertex);

    vec2 inPos = sign(vertex.xy);
    vUV0 = (vec2(inPos.x, -inPos.y) + 1.0) * 0.5;
}
