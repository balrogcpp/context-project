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

void main()
{
  vec3 color = vec3(0.0);
  for (int x = -2; x < 1; x++)
  for (int y = -2; y < 1; y++)
    color += texture2D(uSampler, vec2(oUv0.x + float(x) * TexelSize.x, oUv0.y + float(y) * TexelSize.y)).rgb;
  color /= 9.0;

  FragColor.rgb = color;
}
