/// created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif

#include "header.frag"
#include "filters.glsl"
#ifndef NO_MRT
#ifdef SRGB
#include "srgb.glsl"
#endif // SRGB
#ifdef USE_FOG
#include "fog.glsl"
#endif // USE_FOG
#ifdef USE_HDR
#include "tonemap.glsl"
#endif // USE_HDR
#endif // NO_MRT

#ifndef MAX_SAMPLES
#define MAX_SAMPLES 4
#endif

in vec2 vUV0;
uniform sampler2D uSceneSampler;
uniform vec2 TexelSize;
#ifndef NO_MRT
#ifdef USE_FOG
uniform sampler2D uSceneDepthSampler;
#endif // FOD
#ifdef USE_BLUR
uniform sampler2D uSpeedSampler;
#endif // USE_BLUR
#ifdef USE_SSAO
uniform sampler2D uSsaoSampler;
#endif // USE_SSAO
#ifdef USE_BLOOM
uniform sampler2D uRT1;
uniform sampler2D uRT2;
uniform sampler2D uRT3;
uniform sampler2D uRT4;
uniform sampler2D uRT5;
uniform sampler2D uRT6;
uniform sampler2D uRT7;
uniform sampler2D uRT8;
uniform sampler2D uRT9;
uniform sampler2D uRT10;
uniform sampler2D uRT11;
uniform sampler2D uRT12;
uniform vec2 TexelSize3;
uniform vec2 TexelSize4;
uniform vec2 TexelSize5;
uniform vec2 TexelSize6;
uniform vec2 TexelSize7;
uniform vec2 TexelSize8;
uniform vec2 TexelSize9;
uniform vec2 TexelSize10;
uniform vec2 TexelSize11;
uniform vec2 TexelSize12;
#endif // USE_BLOOM
#ifdef SRGB
#ifdef MANUAL_SRGB
uniform float uExposure;
#endif // MANUAL_SRGB
#endif // SRGB
#ifdef USE_FOG
uniform vec3 FogColour;
uniform vec4 FogParams;
uniform float NearClipDistance;
uniform float FarClipDistance;
#endif // USE_FOG
#ifdef USE_BLUR
uniform vec2 PixelSize;
uniform float uScale;
#endif // USE_BLUR
#endif // !NO_MRT
#ifdef USE_FXAA
uniform float uFXAAStrength;
#endif // USE_FXAA

//----------------------------------------------------------------------------------------------------------------------
void main()
{
#ifndef NO_MRT
  vec3 scene = texture2D(uSceneSampler, vUV0).rgb;
#ifdef USE_SSAO
  {
    float color = texture2D(uSsaoSampler, vUV0).r;
    scene *= clamp(color + 0.1, 0.0, 1.0);
  }
#endif // USE_SSAO
#ifdef USE_BLOOM
  {
    vec3 A = Linear(uRT1, vUV0, TexelSize3);
    vec3 B = Linear(uRT2, vUV0, TexelSize4);
    vec3 C = Upscale3x3(uRT3, vUV0, TexelSize5);
    vec3 D = Upscale3x3(uRT4, vUV0, TexelSize6);
    vec3 E = Upscale3x3(uRT5, vUV0, TexelSize7);
    vec3 F = Upscale3x3(uRT6, vUV0, TexelSize8);
    vec3 G = Upscale3x3(uRT7, vUV0, TexelSize9);
    vec3 H = Upscale3x3(uRT8, vUV0, TexelSize10);
    vec3 I = Upscale3x3(uRT9, vUV0, TexelSize11);
    vec3 K = Upscale3x3(uRT10, vUV0, TexelSize12);

    scene += (A + B + C + D) * 0.65;
    scene += (E + F + G + H) * 0.25;
    scene += (I + K) * 0.25;
#ifdef USE_HDR
    {
      float lum = dot(K, vec3(0.27, 0.67, 0.06));
      scene = tone_map(scene, lum);
    }
#endif // USE_HDR
  }
#endif // USE_BLOOM
#ifdef USE_FOG
  {
    float clampedDepth = texture2D(uSceneDepthSampler, vUV0).r;
    scene = ApplyFog(scene, FogParams, FogColour, clampedDepth * FarClipDistance);
  }
#endif // USE_FOG
#ifdef USE_BLUR
  {
    vec2 velocity = uScale * texture2D(uSpeedSampler, vUV0).rg;
    float speed = length(velocity * PixelSize);
    int nSamples = int(clamp(speed, 1.0, float(MAX_SAMPLES)));
    for (int i = 1; i < MAX_SAMPLES; i++) {
      if (nSamples <= i) break;
      vec2 offset = velocity * (float(i) / float(nSamples - 1) - 0.5);
      scene += texture2D(uSceneSampler, vUV0 + offset).rgb;
    }
    scene /= float(nSamples);
  }
#endif // USE_BLUR
#else // NO_MRT
#ifdef USE_FXAA
  vec3 scene = FXAA(uSceneSampler, vUV0, TexelSize, uFXAAStrength);
#else // !USE_FXAA
  vec3 scene = texture2D(uSceneSampler, vUV0).rgb;
#endif // USE_FXAA
#endif // !NO_MRT
#ifdef MANUAL_SRGB
#ifdef SRGB
  scene = LINEARtoSRGB(scene, uExposure);
#endif // SRGB
#endif // MANUAL_SRGB
  FragColor.rgb = scene;
}
