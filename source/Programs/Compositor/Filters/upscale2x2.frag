// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define VERSION 150
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
  FragColor.rgb = Upscale2x2(uSampler, vUV0, TexelSize);
}
