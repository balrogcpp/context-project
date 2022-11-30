// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif

#include "header.frag"
#include "fog.glsl"

in vec2 vUV0;
uniform sampler2D uSceneSampler;
uniform sampler2D uSceneDepthSampler;
uniform vec3 FogColour;
uniform vec4 FogParams;
uniform float FarClipDistance;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(uSceneSampler, vUV0).rgb;
  float clampedDepth = texture2D(uSceneDepthSampler, vUV0).r;
  vec3 fog = ApplyFog(color, FogParams, FogColour, clampedDepth * FarClipDistance);
  FragColor.rgb = fog;
}
