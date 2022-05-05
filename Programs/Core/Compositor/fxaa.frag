// This source file is part of Glue Engine. Created by Andrey Vasiliev

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
uniform float uFXAAEnable;
uniform sampler2D uSampler;
uniform vec2 TexelSize;
uniform float uFXAAStrength;

//----------------------------------------------------------------------------------------------------------------------
void main()
{
    if (uFXAAEnable > 0.0)
      FragColor.rgb = FXAA(uSampler, vUV0, TexelSize, uFXAAStrength);
    else
     FragColor.rgb = texture2D(uSampler, vUV0).rgb;
}
