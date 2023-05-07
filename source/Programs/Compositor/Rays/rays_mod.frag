// created by Andrey Vasiliev

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "math.glsl"


varying mediump vec2 vUV0;
uniform sampler2D RT;
uniform sampler2D FboMap;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump vec3 rays = texture2D(FboMap, vUV0).rgb;
    FragColor = vec4(SafeHDR(color + rays), 1.0);
}
