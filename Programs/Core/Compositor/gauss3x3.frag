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
void main()
{
  const vec2 offsetx0 = vec2(0.0, 0.0);
  const vec2 offsetx1 = vec2(1.3846153846, 0.0);
  const vec2 offsetx2 = vec2(3.2307692308, 0.0);

  const vec2 offsety0 = vec2(0.0, 0.0);
  const vec2 offsety1 = vec2(0.0, 1.3846153846);
  const vec2 offsety2 = vec2(0.0, 3.2307692308);

  const float weight0 = 0.2270270270;
  const float weight1 = 0.3162162162;
  const float weight2 = 0.0702702703;

  vec3 final_color = vec3(weight0 * texture2D(uSampler, oUv0).rgb);

  final_color += vec3(weight1 * (texture2D(uSampler, (oUv0 + offsetx1 * TexelSize)).rgb + texture2D(uSampler, oUv0 - offsetx1 * TexelSize).rgb));
  final_color += vec3(weight2 * (texture2D(uSampler, (oUv0 + offsetx2 * TexelSize)).rgb + texture2D(uSampler, oUv0 - offsetx2 * TexelSize).rgb));

  final_color += vec3(weight1 * (texture2D(uSampler, oUv0 + offsety1 * TexelSize).rgb + texture2D(uSampler, oUv0 - offsety1 * TexelSize).rgb));
  final_color += vec3(weight2 * (texture2D(uSampler, oUv0 + offsety2 * TexelSize).rgb + texture2D(uSampler, oUv0 - offsety2 * TexelSize).rgb));

  FragColor.rgb = final_color;
}
