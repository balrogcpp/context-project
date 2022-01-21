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
uniform sampler2D SceneSampler;

void main()
{
  vec3 color = texture2D(SceneSampler, oUv0, 1.0).rgb;
  vec3 bloom = vec3(0.0);

  float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
  if(brightness > 1.0)
    bloom = color;

  const float scale = 0.01;

  gl_FragColor = vec4(scale * bloom, 1.0);
}
