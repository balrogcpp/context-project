// This source file is part of "glue project". Created by Andrey Vasiliev

#ifndef GL_ES
#define VERSION 120
#version VERSION
#else
#define VERSION 100
#version VERSION
#endif
#include "header.frag"

in vec2 oUv0;
uniform sampler2D uSampler;
uniform vec2 texelSize;

void main()
{
  float color = 0.0;
  for (int x = -2; x < 2; x++)
  for (int y = -2; y < 2; y++)
  {
    color += texture2D(uSampler, vec2(oUv0.x + float(x) * texelSize.x, oUv0.y + float(y) * texelSize.y)).x;
  }
  color /= 16.0;

  gl_FragColor = vec4(color, 0.0, 0.0, 1.0);
}
