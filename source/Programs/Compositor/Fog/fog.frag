// created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
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
  vec3 scene = texture2D(uSceneSampler, vUV0).rgb;
  float clampedDepth = texture2D(uSceneDepthSampler, vUV0).r;
  FragColor.rgb = ApplyFog(scene, FogParams, FogColour, clampedDepth * FarClipDistance);
}
