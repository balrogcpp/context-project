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

  vec2 offset0 = TexelSize * vec2(0.0, 0.0);
  vec2 offset1 = TexelSize * vec2(0.0, 1.0);
  vec2 offset2 = TexelSize * vec2(0.0, 2.0);
  vec2 offset3 = TexelSize * vec2(0.0, 3.0);
  vec2 offset4 = TexelSize * vec2(0.0, 4.0);

  const float weight0 = 0.2270270270;
  const float weight1 = 0.1945945946;
  const float weight2 = 0.1216216216;
  const float weight3 = 0.0540540541;
  const float weight4 = 0.0162162162;

  vec3 color = weight0 * texture2D(uSampler, oUv0).rgb;

  color += weight1 * (texture2D(uSampler, oUv0 + offset1).rgb + texture2D(uSampler, oUv0 - offset1).rgb);
  color += weight2 * (texture2D(uSampler, oUv0 + offset2).rgb + texture2D(uSampler, oUv0 - offset2).rgb);
  color += weight3 * (texture2D(uSampler, oUv0 + offset3).rgb + texture2D(uSampler, oUv0 - offset3).rgb);
  color += weight4 * (texture2D(uSampler, oUv0 + offset4).rgb + texture2D(uSampler, oUv0 - offset4).rgb);

  FragColor.rgb = color;
}
