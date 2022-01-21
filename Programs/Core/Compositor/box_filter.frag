// This source file is part of "glue project". Created by Andrey Vasiliev

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

void main()
{
  vec3 color = vec3(0.0);

  for (int x = -2; x < 1; x++)
  for (int y = -2; y < 1; y++)
  {
    color += texture2D(uSampler, vec2(oUv0.x + float(x) * texelSize.x, oUv0.y + float(y) * texelSize.y)).rgb;
  }
  color /= 9.0;

  FragColor = vec4(color, 1.0);
}
