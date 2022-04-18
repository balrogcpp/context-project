// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif

#include "header.frag"
#include "srgb.glsl"

#define KERNEL_SIZE 9
#define THRESHOLD 0.9999

in vec2 oUv0;
uniform float uBloomEnable;
uniform sampler2D uSceneSampler;
uniform vec2 TexelSize;

void main()
{
  if (uBloomEnable <= 0.0) {
    gl_FragColor.rgb = vec3(0.0);
   return;
  }

  float weights0 = 1.0/16.0;
  float weights1 = 2.0/16.0;
  float weights2 = 1.0/16.0;
  float weights3 = 2.0/16.0;
  float weights4 = 4.0/16.0;
  float weights5 = 2.0/16.0;
  float weights6 = 1.0/16.0;
  float weights7 = 2.0/16.0;
  float weights8 = 1.0/16.0;

  vec2 offsets0 = vec2(-TexelSize.x, -TexelSize.y);
  vec2 offsets1 = vec2(0.0, -TexelSize.y);
  vec2 offsets2 = vec2(TexelSize.x, -TexelSize.y);
  vec2 offsets3 = vec2(-TexelSize.x, 0.0);
  vec2 offsets4 = vec2(0.0, 0.0);
  vec2 offsets5 = vec2(TexelSize.x, 0.0);
  vec2 offsets6 = vec2(-TexelSize.x, TexelSize.y);
  vec2 offsets7 = vec2(0.0,  TexelSize.y);
  vec2 offsets8 = vec2(TexelSize.x, TexelSize.y);

  vec3 color = vec3(0.0);

  color += (weights0 * texture2D(uSceneSampler, oUv0 + offsets0).rgb);
  color += (weights1 * texture2D(uSceneSampler, oUv0 + offsets1).rgb);
  color += (weights2 * texture2D(uSceneSampler, oUv0 + offsets2).rgb);
  color += (weights3 * texture2D(uSceneSampler, oUv0 + offsets3).rgb);
  color += (weights4 * texture2D(uSceneSampler, oUv0 + offsets4).rgb);
  color += (weights5 * texture2D(uSceneSampler, oUv0 + offsets5).rgb);
  color += (weights6 * texture2D(uSceneSampler, oUv0 + offsets6).rgb);
  color += (weights7 * texture2D(uSceneSampler, oUv0 + offsets7).rgb);
  color += (weights8 * texture2D(uSceneSampler, oUv0 + offsets8).rgb);

  vec3 hdr = vec3(0.0);
  if(color.x > THRESHOLD || color.y > THRESHOLD || color.z > THRESHOLD) hdr = color;
  gl_FragColor.rgb = hdr;
}
