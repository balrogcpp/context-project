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

in vec2 oUv0;
uniform sampler2D SceneSampler;

void main()
{
  vec3 hdr = vec3(0.0);
  vec3 color = texture2D(SceneSampler, oUv0, 0.0).rgb;
  const float theshhold = 0.9999;
  if(color.x > theshhold || color.y > theshhold || color.z > theshhold) hdr = color;
  gl_FragColor.rgb = hdr;
}
