// This source file is part of Glue Engine. Created by Andrey Vasiliev

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
#ifdef USE_BLOOM
#include "filters.glsl"
#endif // USE_BLOOM
#ifdef USE_SSAO
#include "filters.glsl"
#endif // USE_SSAO
//#ifdef USE_HDR
//#include "tonemap.glsl"
//#endif // USE_HDR
#endif // NO_MRT

#ifndef MAX_SAMPLES
#define MAX_SAMPLES 8
#endif

in vec2 vUV0;
uniform sampler2D uSceneSampler;
uniform vec2 TexelSize;
#ifndef NO_MRT
#ifdef USE_FOG
uniform sampler2D uSceneDepthSampler;
#endif // FOD
#ifdef MOTION_BLUR
uniform sampler2D uSpeedSampler;
#endif // MOTION_BLUR
#ifdef USE_SSAO
uniform sampler2D uSsaoSampler;
uniform float uSSAOEnable;
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
uniform float uBloomEnable;
#endif // USE_BLOOM
#ifdef SRGB
#ifdef MANUAL_SRGB
uniform float uExposure;
#endif // MANUAL_SRGB
#endif // SRGB
#ifdef USE_FOG
uniform vec3 FogColour;
uniform vec4 FogParams;
uniform float nearClipDistance;
uniform float FarClipDistance;
#endif // USE_FOG
#ifdef MOTION_BLUR
uniform vec2 PixelSize;
uniform float uScale;
uniform float uBlurEnable;
#endif // MOTION_BLUR
//#ifdef USE_HDR
//uniform sampler2D uRT8;
//uniform float uHDREnable;
//#endif // USE_HDR
#endif // !NO_MRT
#ifdef USE_FXAA
uniform float uFXAAStrength;
uniform float uFXAAEnable;
#endif // USE_FXAA

//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 scene = texture2D(uSceneSampler, vUV0).rgb;
#ifndef NO_MRT
#ifdef USE_SSAO
  if (uSSAOEnable > 0.0)
  {
    float color = BoxFilterR(uSsaoSampler, vUV0, TexelSize);
    scene *= clamp(color + 0.1, 0.0, 1.0);
  }
#endif // USE_SSAO
#ifdef USE_BLOOM
  if (uBloomEnable > 0.0)
  {
    vec3 A = Upscale3x3(uRT1, vUV0, TexelSize3);
    vec3 B = Upscale3x3(uRT2, vUV0, TexelSize4);
    vec3 C = Upscale3x3(uRT3, vUV0, TexelSize5);
    vec3 D = Upscale3x3(uRT4, vUV0, TexelSize6);
    vec3 E = Upscale3x3(uRT5, vUV0, TexelSize7);
    vec3 F = Upscale3x3(uRT6, vUV0, TexelSize8);
    vec3 G = Upscale3x3(uRT7, vUV0, TexelSize9);
    vec3 H = Upscale3x3(uRT8, vUV0, TexelSize10);
    vec3 I = Upscale3x3(uRT9, vUV0, TexelSize11);
    vec3 K = Upscale3x3(uRT10, vUV0, TexelSize12);

    scene += (A + B + C + D) * 0.65;
    scene += (E + F + G + H) * 0.35;
    scene += (I + K) * 0.25;
  }
#endif // USE_BLOOM
#ifdef USE_FOG
  {
    float clampedDepth = texture2D(uSceneDepthSampler, vUV0).r;
    float fragmentWorldDepth = clampedDepth * FarClipDistance;
    scene = ApplyFog(scene, FogParams, FogColour, fragmentWorldDepth);
  }
#endif // USE_FOG
//#ifdef USE_HDR
//  if (uHDREnable > 0.0)
//  {
//    float lum = dot(scene, vec3(0.27, 0.67, 0.06));
//    scene = tone_map(scene, lum);
//  }
//#endif // USE_HDR
#endif // !NO_MRT
#ifndef NO_MRT
#ifdef MOTION_BLUR
  if (uBlurEnable > 0.0)
  {
    vec2 velocity = uScale * texture2D(uSpeedSampler, vUV0).rg;
    float speed = length(velocity * PixelSize);
    int nSamples = int(clamp(speed, 1.0, float(MAX_SAMPLES)));
    for (int i = 1; i < nSamples; i++) {
      vec2 offset = velocity * (float(i) / float(nSamples - 1) - 0.5);
      vec2 uv = vUV0 + offset;
      scene += texture2D(uSceneSampler, uv).rgb;
    }
    scene /= float(nSamples);
  }
#endif // MOTION_BLUR
#endif // !NO_MRT
#ifdef MANUAL_SRGB
#ifdef SRGB
  scene = LINEARtoSRGB(scene, uExposure);
#endif // SRGB
#endif // MANUAL_SRGB
  FragColor.rgb = scene;
}
