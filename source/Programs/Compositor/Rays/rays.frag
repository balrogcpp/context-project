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


in vec2 vUV0;

uniform sampler2D uFBO;
uniform float LightCount;
uniform vec4 LightPositionViewSpace[MAX_LIGHTS];
uniform int uRayCount;
uniform float uDecay;
uniform float uDensity;
uniform float uWeight;
uniform float uExposure;


//----------------------------------------------------------------------------------------------------------------------
vec3 GodRays(const sampler2D tex, const vec2 uv, const vec2 lightPos, const int counter, const float density, const float weight, const float decay, const float exposure)
{
  vec3 color = vec3(0.0);
  vec2 suv = uv.st;
  float illuminationDecay = 1.0;
  vec2 deltaTextCoord = (uv - lightPos) * (1.0 / float(counter)) * density;
  #define MAX_RAYS 100

  for(int i = 0; i < MAX_RAYS; ++i) {
    if (counter <= i) break;

    suv -= deltaTextCoord;
    color += texture2D(tex, suv).rgb * (illuminationDecay * weight);
    illuminationDecay *= decay;
  }

  return expose(color, exposure);
}



//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = vec3(0.0);

  for (int i = 0; i < MAX_LIGHTS; ++i) {
    if (int(LightCount) <= i) break;

    vec4 point = LightPositionViewSpace[i];
    color += GodRays(uFBO, vUV0, point.xy, uRayCount, uDensity, uWeight * point.z, uDecay, uExposure);
  }

  FragColor.rgb = SafeHDR(color);
}
