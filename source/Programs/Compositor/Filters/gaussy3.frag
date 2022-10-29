// created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif

#include "header.frag"
#include "filters.glsl"

in vec2 vUV0;
uniform sampler2D uSampler;
uniform vec2 TexelSize;

//----------------------------------------------------------------------------------------------------------------------
void main()
{
  FragColor.rgb = GaussV(uSampler, vUV0, TexelSize);
}