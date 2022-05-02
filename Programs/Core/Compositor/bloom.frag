// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif

#include "header.frag"
#include "downscale3x3.glsl"

#define THRESHOLD 0.9999

in vec2 oUv0;
uniform float uEnable;
uniform sampler2D uSampler;
uniform vec2 TexelSize;

void main()
{
  if (uEnable <= 0.0) discard;

  vec3 color = Downscale3x3(uSampler, oUv0, TexelSize);

  vec3 hdr = vec3(0.0);
  if(color.x > THRESHOLD || color.y > THRESHOLD || color.z > THRESHOLD) hdr = color;

  FragColor.rgb = hdr;
}
