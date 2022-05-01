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

  const vec2 offsetx0 = vec2(0.0, 0.0);
  const vec2 offsetx1 = vec2(1.0, 0.0);
  const vec2 offsetx2 = vec2(2.0, 0.0);
  const vec2 offsetx3 = vec2(3.0, 0.0);
  const vec2 offsetx4 = vec2(4.0, 0.0);

  const vec2 offsety0 = vec2(0.0, 0.0);
  const vec2 offsety1 = vec2(0.0, 1.0);
  const vec2 offsety2 = vec2(0.0, 2.0);
  const vec2 offsety3 = vec2(0.0, 3.0);
  const vec2 offsety4 = vec2(0.0, 4.0);

  const float weight0 = 0.2270270270;
  const float weight1 = 0.1945945946;
  const float weight2 = 0.1216216216;
  const float weight3 = 0.0540540541;
  const float weight4 = 0.0162162162;

  vec3 final_color = vec3(weight0 * texture2D(uSampler, oUv0).rgb);

  final_color += vec3(weight1 * (texture2D(uSampler, oUv0 + offsetx1 * TexelSize).rgb + texture2D(uSampler, oUv0 - offsetx1 * TexelSize).rgb));
  final_color += vec3(weight2 * (texture2D(uSampler, oUv0 + offsetx2 * TexelSize).rgb + texture2D(uSampler, oUv0 - offsetx2 * TexelSize).rgb));
  final_color += vec3(weight3 * (texture2D(uSampler, oUv0 + offsetx3 * TexelSize).rgb + texture2D(uSampler, oUv0 - offsetx3 * TexelSize).rgb));
  final_color += vec3(weight4 * (texture2D(uSampler, oUv0 + offsetx4 * TexelSize).rgb + texture2D(uSampler, oUv0 - offsetx4 * TexelSize).rgb));

  final_color += vec3(weight1 * (texture2D(uSampler, oUv0 + offsety1 * TexelSize).rgb + texture2D(uSampler, oUv0 - offsety1 * TexelSize).rgb));
  final_color += vec3(weight2 * (texture2D(uSampler, oUv0 + offsety2 * TexelSize).rgb + texture2D(uSampler, oUv0 - offsety2 * TexelSize).rgb));
  final_color += vec3(weight3 * (texture2D(uSampler, oUv0 + offsety3 * TexelSize).rgb + texture2D(uSampler, oUv0 - offsety3 * TexelSize).rgb));
  final_color += vec3(weight4 * (texture2D(uSampler, oUv0 + offsety4 * TexelSize).rgb + texture2D(uSampler, oUv0 - offsety4 * TexelSize).rgb));

  FragColor.rgb = final_color;
}
