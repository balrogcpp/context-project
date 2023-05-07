// created by Andrey Vasiliev

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "filters.glsl"


varying mediump vec2 vUV0;
uniform sampler2D RT;
uniform mediump vec2 TexelSize0;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    FragColor = vec4(Gauss9V(RT, vUV0, TexelSize0), 1.0);
}
