// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"
#include "math.glsl"
#include "srgb.glsl"


//----------------------------------------------------------------------------------------------------------------------
vec3 GodRays(const sampler2D tex, const vec2 uv, const vec2 lightPos, const int counter, const float density, const float weight, const float decay, const float exposure)
{
  vec3 color = vec3(0.0);
  vec2 suv = uv.st;
  float illuminationDecay = 1.0;
  vec2 deltaTextCoord = (uv - lightPos) * (1.0 /  float(counter)) * density;
  #define MAX_RAYS 100

  for(int i = 0; i < MAX_RAYS; ++i) {
	if (counter <= i) break;

	suv -= deltaTextCoord;
	color += texture2D(tex, suv).rgb * (illuminationDecay * weight);
	illuminationDecay *= decay;
  }

  return expose(color, exposure);
}


in vec2 vUV0;

uniform sampler2D uSampler;
uniform sampler2D uFBO;
uniform vec2 TexelSize0;
uniform float LightCount;
uniform vec4 LightPositionViewSpace[MAX_LIGHTS];
uniform int uRayCount;
uniform float uDecay;
uniform float uDensity;
uniform float uWeight;
uniform float uExposure;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(uSampler, vUV0).rgb;

  for (int i = 0; i < MAX_LIGHTS; ++i) {
    if (int(LightCount) <= i) break;

    color += GodRays(uFBO, vUV0, LightPositionViewSpace[i].xy, uRayCount, uDensity, uWeight, uDecay, uExposure);
  }

  FragColor.rgb = color;
}
