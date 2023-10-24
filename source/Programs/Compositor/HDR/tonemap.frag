// created by Andrey Vasiliev

#ifndef __VERSION__
#if defined(OGRE_GLSL)
#version 330 core
#define __VERSION__ 330
#elif defined(OGRE_GLSLES)
#version 300 es
#define __VERSION__ 300
#endif
#endif

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;

in vec2 vUV0;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
    color *= 2.0;
//    color = expose(color, 5.0);
    FragColor.rgb = SafeHDR(unreal(color));
}
