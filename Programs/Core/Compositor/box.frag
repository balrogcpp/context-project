// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif

#include "header.frag"
#include "box.glsl"

in vec2 oUv0;
uniform sampler2D uSampler;
uniform vec2 TexelSize;

//----------------------------------------------------------------------------------------------------------------------
void main()
{
  FragColor.rgb = BoxFilter(uSampler, oUv0, TexelSize);
}
