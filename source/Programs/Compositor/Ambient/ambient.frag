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
uniform sampler2D AmbientMap;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump vec3 ambient = texture2D(AmbientMap, vUV0).rgb;

    FragColor = vec4(SafeHDR(color + ambient), 1.0);
}
