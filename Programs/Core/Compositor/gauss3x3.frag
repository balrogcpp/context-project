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
uniform vec2 TexelSize;

//----------------------------------------------------------------------------------------------------------------------
vec3 IterationX(const float offset, const float weight)
{
    return weight * (texture2D(uSampler, (oUv0 + vec2(offset, 0.0) * TexelSize.x)).rgb + texture2D(uSampler, (oUv0 - vec2(offset, 0.0) * TexelSize.x)).rgb);
}

//----------------------------------------------------------------------------------------------------------------------
vec3 IterationY(const float offset, const float weight)
{
    return vec3(weight * (texture2D(uSampler, (oUv0 + vec2(0.0, offset) * TexelSize.y)).rgb + texture2D(uSampler, (oUv0 - vec2(0.0, offset) * TexelSize.y)).rgb));
}

//----------------------------------------------------------------------------------------------------------------------
void main()
{
  const float offset0 = 0.0;
  const float offset1 = 1.3846153846;
  const float offset2 = 3.2307692308;

  const float weight0 = 0.2270270270;
  const float weight1 = 0.3162162162;
  const float weight2 = 0.0702702703;

  vec3 final_color = vec3(weight0 * texture2D(uSampler, oUv0).rgb);

  final_color += IterationX(offset1, weight1);
  final_color += IterationX(offset2, weight2);

  final_color += IterationY(offset1, weight1);
  final_color += IterationY(offset2, weight2);

  FragColor.rgb = final_color;
}
