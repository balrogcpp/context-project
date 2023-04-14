// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "filters.glsl"


in vec2 vUV0;
uniform sampler2D RT;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    FragColor.rgba = texture2D(RT, vUV0);
}
