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


#ifndef MAX_LIGHTS
#define MAX_LIGHTS 1
#endif


//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L15
vec3 Downscale4x4(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
  vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).rgb;
  vec3 B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).rgb;
  vec3 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).rgb;
  vec3 D = texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).rgb;
  vec3 E = texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).rgb;
  vec3 F = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).rgb;
  vec3 G = texture2D(tex, uv                           ).rgb;
  vec3 H = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).rgb;
  vec3 I = texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).rgb;
  vec3 J = texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).rgb;
  vec3 K = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).rgb;
  vec3 L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).rgb;
  vec3 M = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).rgb;

  vec3 color = (D + E + I + J) * 0.125;
  color += (A + B + G + F) * 0.03125;
  color += (B + C + H + G) * 0.03125;
  color += (F + G + L + K) * 0.03125;
  color += (G + H + M + L) * 0.03125;

  return color;
}


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
  //vec3 color = Downscale4x4(uSampler, vUV0, TexelSize0);
  vec3 color = texture2D(uSampler, vUV0).rgb;

  for (int i = 0; i < MAX_LIGHTS; ++i) {
    if (int(LightCount) <= i) break;

    color += GodRays(uSampler, vUV0, LightPositionViewSpace[i].xy, uRayCount, uDensity, uWeight, uDecay, uExposure);
  }

  FragColor.rgb = color;
}
