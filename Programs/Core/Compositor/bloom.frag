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
uniform float uEnable;
uniform sampler2D uSampler;
uniform vec2 TexelSize;
uniform float uThreshhold;

//----------------------------------------------------------------------------------------------------------------------
vec3 ApplyThreshold(const vec3 color, const float threshold)
{
  const float strength = 1.0;
  // convert rgb to grayscale/brightness
  float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
  if (brightness > threshold) return strength * color;
  else return vec3(0.0);
}

//----------------------------------------------------------------------------------------------------------------------
void main()
{
  if (uEnable <= 0.0) discard;
  FragColor.rgb = ApplyThreshold(texture2D(uSampler, vUV0).rgb, uThreshhold) ;
}
