// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif

#include "header.frag"

in vec2 oUv0;
uniform float uEnable;
uniform sampler2D uSampler;
uniform vec2 TexelSize;

//----------------------------------------------------------------------------------------------------------------------
void main()
{
  if (uEnable <= 0.0) discard;

  const vec2 offset0 = vec2(0.0, 0.0);
  const vec2 offset1 = vec2(0.0, 1.3846153846);
  const vec2 offset2 = vec2(0.0, 3.2307692308);

  const float weight0 = 0.2270270270;
  const float weight1 = 0.3162162162;
  const float weight2 = 0.0702702703;

  vec3 color = weight0 * texture2D(uSampler, oUv0).rgb;

  color += weight1 * (texture2D(uSampler, oUv0 + (offset1 * TexelSize)).rgb + texture2D(uSampler, oUv0 - (offset1 * TexelSize)).rgb);
  color += weight2 * (texture2D(uSampler, oUv0 + (offset2 * TexelSize)).rgb + texture2D(uSampler, oUv0 - (offset2 * TexelSize)).rgb);

  FragColor.rgb = color;
}
