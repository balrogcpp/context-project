// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"
#include "filters.glsl"


in vec2 vUV0;
uniform sampler2D uSceneSampler;
uniform sampler2D uSsaoSampler;
uniform vec2 TexelSize1;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(uSceneSampler, vUV0).rgb;
  float ssao = texture2D(uSsaoSampler, vUV0).r;
  //float ssao = BoxFilter4R(uSsaoSampler, vUV0, TexelSize1).r;
  color *= clamp(ssao, 0.0, 1.0);
  FragColor.rgb = color;
}
