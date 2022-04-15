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
uniform sampler2D uSampler;
uniform vec2 texelSize;

//----------------------------------------------------------------------------------------------------------------------
vec3 IterationX(const float offset, const float weight)
{
  return weight * (texture2D(uSampler, (oUv0 + vec2(offset, 0.0) * texelSize.x)).rgb + texture2D(uSampler, (oUv0 - vec2(offset, 0.0) * texelSize.x)).rgb);
}

//----------------------------------------------------------------------------------------------------------------------
vec3 IterationY(const float offset, const float weight)
{
  return weight * (texture2D(uSampler, (oUv0 + vec2(0.0, offset) * texelSize.y)).rgb + texture2D(uSampler, (oUv0 - vec2(0.0, offset) * texelSize.y)).rgb);
}

//----------------------------------------------------------------------------------------------------------------------
void main()
{
  const float offset0 = 0.0;
  const float offset1 = 1.0;
  const float offset2 = 2.0;
  const float offset3 = 3.0;
  const float offset4 = 4.0;

  const float weight0 = 0.2270270270;
  const float weight1 = 0.1945945946;
  const float weight2 = 0.1216216216;
  const float weight3 = 0.0540540541;
  const float weight4 = 0.0162162162;

  vec3 final_color = (weight0 * texture2D(uSampler, oUv0).rgb);

  final_color += IterationX(offset1, weight1);
  final_color += IterationX(offset2, weight2);
  final_color += IterationX(offset3, weight3);
  final_color += IterationX(offset4, weight4);

  final_color += IterationY(offset1, weight1);
  final_color += IterationY(offset2, weight2);
  final_color += IterationY(offset3, weight3);
  final_color += IterationY(offset4, weight4);

  FragColor.rgb = final_color;
}
