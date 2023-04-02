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
uniform sampler2D uColorMap;
uniform sampler2D uDepthMap;
uniform vec3 FogColour;
uniform vec4 FogParams;
uniform float FarClipDistance;
uniform float NearClipDistance;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(uColorMap, vUV0).rgb;
  float clampedDepth = texture2D(uDepthMap, vUV0).x;
  if (clampedDepth > 0.5)
  {
    FragColor.rgb = color;
    return;
  }
  float depth = clampedDepth * FarClipDistance + NearClipDistance;
  vec3 fog = ApplyFog(color, FogParams, FogColour, depth);
  FragColor.rgb = fog;
}
