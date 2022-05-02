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

  const vec3 weights0 = vec3(0.0625); // 1/16
  const vec3 weights1 = vec3(0.125); // 2/16
  const vec3 weights2 = vec3(0.0625); // 1/16
  const vec3 weights3 = vec3(0.125); // 2/16
  const vec3 weights4 = vec3(0.25); // 4/16
  const vec3 weights5 = vec3(0.125); // 2/16
  const vec3 weights6 = vec3(0.0625); // 1/16
  const vec3 weights7 = vec3(0.125); // 2/16
  const vec3 weights8 = vec3(0.0625); // 1/16

  const vec2 offsets0 = vec2(-1.0, -1.0);
  const vec2 offsets1 = vec2(0.0, -1.0);
  const vec2 offsets2 = vec2(1.0, -1.0);
  const vec2 offsets3 = vec2(-1.0, 0.0);
  const vec2 offsets4 = vec2(0.0, 0.0);
  const vec2 offsets5 = vec2(1.0, 0.0);
  const vec2 offsets6 = vec2(-1.0, 1.0);
  const vec2 offsets7 = vec2(0.0,  1.0);
  const vec2 offsets8 = vec2(1.0, 1.0);

  vec3 color = vec3(0.0);

  color += weights0 * texture2D(uSampler, oUv0 + offsets0 * TexelSize).rgb;
  color += weights1 * texture2D(uSampler, oUv0 + offsets1 * TexelSize).rgb;
  color += weights2 * texture2D(uSampler, oUv0 + offsets2 * TexelSize).rgb;
  color += weights3 * texture2D(uSampler, oUv0 + offsets3 * TexelSize).rgb;
  color += weights4 * texture2D(uSampler, oUv0 + offsets4 * TexelSize).rgb;
  color += weights5 * texture2D(uSampler, oUv0 + offsets5 * TexelSize).rgb;
  color += weights6 * texture2D(uSampler, oUv0 + offsets6 * TexelSize).rgb;
  color += weights7 * texture2D(uSampler, oUv0 + offsets7 * TexelSize).rgb;
  color += weights8 * texture2D(uSampler, oUv0 + offsets8 * TexelSize).rgb;

  FragColor.rgb = color;
}
