// created by Andrey Vasiliev

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"


in mediump vec2 vUV0;
uniform sampler2D RT;
uniform sampler2D SsaoMap;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump float ssao = texture2D(SsaoMap, vUV0).r;
    color *= ssao;
    FragColor = vec4(color, 1.0);
}
