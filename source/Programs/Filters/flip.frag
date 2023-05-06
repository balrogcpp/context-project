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


in mediump vec2 vUV0;
uniform sampler2D RT;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    vec2 uv = vec2(vUV0.s, 1.0 - vUV0.t);
    FragColor = vec4(texture2D(RT, uv).rgb, 1.0);
}
