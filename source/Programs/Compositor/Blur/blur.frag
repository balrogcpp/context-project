/// created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif

#include "header.frag"
#include "srgb.glsl"

#ifndef MAX_SAMPLES
#define MAX_SAMPLES 4
#endif


in vec2 vUV0;
uniform sampler2D uSceneSampler;
uniform sampler2D uSpeedSampler;
uniform vec2 TexelSize0;
uniform vec2 PixelSize1;
uniform float uScale;

#ifdef SRGB
#ifdef MANUAL_SRGB
uniform float uExposure;
#endif
#endif


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 scene = texture2D(uSceneSampler, vUV0).rgb;
  vec2 velocity = uScale * texture2D(uSpeedSampler, vUV0).rg;
  float speed = length(velocity * PixelSize1);
  int nSamples = int(clamp(speed, 1.0, float(MAX_SAMPLES)));
  for (int i = 1; i < MAX_SAMPLES; i++) {
    if (nSamples <= i) break;
    vec2 offset = velocity * (float(i) / float(nSamples - 1) - 0.5);
    scene += texture2D(uSceneSampler, vUV0 + offset).rgb;
  }
  scene /= float(nSamples);
#ifdef MANUAL_SRGB
#ifdef SRGB
  scene = LINEARtoSRGB(scene, uExposure);
#endif
#endif
    FragColor.rgb = scene;
}
