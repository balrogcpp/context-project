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
uniform float uHDREnable;
uniform sampler2D uSceneSampler;
uniform vec2 TexelSize;

//----------------------------------------------------------------------------------------------------------------------
void main()
{
  if (uHDREnable <= 0.0) discard;
  
  const float weights0 = 0.25; // 1/4
  const float weights1 = 0.25; // 1/4
  const float weights2 = 0.25; // 1/4
  const float weights3 = 0.25; // 1/4

  const vec2 offsets0 = vec2(-0.5, -0.5);
  const vec2 offsets1 = vec2(-0.5, 0.5);
  const vec2 offsets2 = vec2(-0.5, 0.5);
  const vec2 offsets3 = vec2(0.5, -0.5);

  vec3 color = vec3(0.0);
  
  color += vec3(weights0 * texture2D(uSceneSampler, oUv0 + offsets0 * TexelSize).rgb);
  color += vec3(weights1 * texture2D(uSceneSampler, oUv0 + offsets1 * TexelSize).rgb);
  color += vec3(weights2 * texture2D(uSceneSampler, oUv0 + offsets2 * TexelSize).rgb);
  color += vec3(weights3 * texture2D(uSceneSampler, oUv0 + offsets3 * TexelSize).rgb);

  FragColor.rgb = color;
}
