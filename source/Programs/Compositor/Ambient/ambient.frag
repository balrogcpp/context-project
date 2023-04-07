// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"


in mediump vec2 vUV0;
uniform mediump sampler2D ColorMap;
uniform mediump sampler2D AmbientMap;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(ColorMap, vUV0).rgb;
    mediump vec3 ambient = texture2D(AmbientMap, vUV0).rgb;
    FragColor.rgb = color + ambient;
}
