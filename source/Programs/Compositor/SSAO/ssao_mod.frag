// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"


in vec2 vUV0;
uniform sampler2D uSceneSampler;
uniform sampler2D uSsaoSampler;
uniform vec2 TexelSize1;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 scene = texture2D(uSceneSampler, vUV0).rgb;
  float color = texture2D(uSsaoSampler, vUV0).r;
  scene *= clamp(color, 0.0, 1.0);
  FragColor.rgb = scene;
}
