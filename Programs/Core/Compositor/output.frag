// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif

#include "header.frag"
#ifndef NO_MRT
#ifdef SRGB
#include "srgb.glsl"
#endif // SRGB
#ifdef FOG
#include "fog.glsl"
#endif // FOG
#ifdef BLOOM
#include "upscale3x3.glsl"
#endif // BLOOM
#ifdef HDR
#include "hdr_tonemap_util.glsl"
#endif // HDR
#endif // NO_MRT

#ifndef MAX_SAMPLES
#define MAX_SAMPLES 8
#endif

in vec2 oUv0;
uniform sampler2D uSceneSampler;
uniform vec2 TexelSize;
#ifndef NO_MRT
#ifdef FOG
uniform sampler2D uSceneDepthSampler;
#endif // FOD
#ifdef MOTION_BLUR
uniform sampler2D uSpeedSampler;
#endif // MOTION_BLUR
#ifdef SSAO
uniform sampler2D uSsaoSampler;
uniform float uSSAOEnable;
#endif // SSAO
#ifdef BLOOM
uniform sampler2D uRT1;
uniform sampler2D uRT2;
uniform sampler2D uRT3;
uniform sampler2D uRT4;
uniform sampler2D uRT5;
uniform sampler2D uRT6;
uniform sampler2D uRT7;
uniform sampler2D uRT8;
uniform vec2 TexelSize3;
uniform vec2 TexelSize4;
uniform vec2 TexelSize5;
uniform vec2 TexelSize6;
uniform vec2 TexelSize7;
uniform vec2 TexelSize8;
uniform vec2 TexelSize9;
uniform vec2 TexelSize10;
uniform float uBloomEnable;
#endif // BLOOM
#ifdef SRGB
#ifdef MANUAL_SRGB
uniform float uExposure;
#endif // MANUAL_SRGB
#endif // SRGB
#ifdef FOG
uniform vec3 FogColour;
uniform vec4 FogParams;
uniform float nearClipDistance;
uniform float FarClipDistance;
#endif // FOG
#ifdef MOTION_BLUR
uniform vec2 PixelSize;
uniform float uScale;
uniform float uBlurEnable;
#endif // MOTION_BLUR
#ifdef HDR
uniform sampler2D uRT8;
uniform float uHDREnable;
#endif // HDR
#endif // !NO_MRT
#ifdef FXAA
uniform float uFXAAStrength;
uniform float uFXAAEnable;
#endif // FXAA

//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 scene = texture2D(uSceneSampler, oUv0).rgb;
#ifndef NO_MRT
#ifdef SSAO
  if (uSSAOEnable > 0.0)
  {
    float color = 0.0;
    for (int x = -2; x < 1; x++)
    for (int y = -2; y < 1; y++)
      color += texture2D(uSsaoSampler, vec2(oUv0.x + float(x) * TexelSize.x, oUv0.y + float(y) * TexelSize.y)).r;
    color *= 0.11111111111111111111; // 1/9
    scene *= vec3(clamp(color + 0.1, 0.0, 1.0));
  }
#endif // SSAO
#ifdef BLOOM
  if (uBloomEnable > 0.0)
  {
    const vec3 weight1 = vec3(0.65);
    const vec3 weight2 = vec3(0.65);
    const vec3 weight3 = vec3(0.65);
    const vec3 weight4 = vec3(0.65);
    const vec3 weight5 = vec3(0.65);
    const vec3 weight6 = vec3(0.65);

    scene += weight1 * Upscale3x3(uRT1, oUv0, TexelSize3);
    scene += weight2 * Upscale3x3(uRT2, oUv0, TexelSize4);
    scene += weight3 * Upscale3x3(uRT3, oUv0, TexelSize5);
    scene += weight4 * Upscale3x3(uRT4, oUv0, TexelSize6);
    scene += weight5 * Upscale3x3(uRT5, oUv0, TexelSize7);
    scene += weight6 * Upscale3x3(uRT6, oUv0, TexelSize8);
    scene += weight6 * Upscale3x3(uRT7, oUv0, TexelSize9);
    scene += weight6 * texture2D(uRT8, vec2(0.5, 0.5)).rgb;
  }
#endif // BLOOM
#ifdef FOG
  {
    float clampedDepth = texture2D(uSceneDepthSampler, oUv0).r;
    float fragmentWorldDepth = clampedDepth * FarClipDistance;
    scene = ApplyFog(scene, FogParams, FogColour, fragmentWorldDepth);
  }
#endif // FOG
#endif // !NO_MRT
#ifdef HDR
  if (uHDREnable > 0.0)
  {
    float lum = dot(scene, vec3(0.27, 0.67, 0.06));
    scene = tone_map(scene, lum);
  }
#endif // HDR
#ifdef FXAA
  if (uFXAAEnable > 0.0)
  {
    const float reducemul = 0.125; // 1/8
    const float reducemin = 0.0078125; // 1/128
    const vec3 gray = vec3(0.299, 0.587, 0.114);

    vec3 basecol = texture2D(uSceneSampler, oUv0).rgb;
    vec3 baseNW = texture2D(uSceneSampler, oUv0 - TexelSize).rgb;
    vec3 baseNE = texture2D(uSceneSampler, oUv0 + vec2(TexelSize.x, -TexelSize.y)).rgb;
    vec3 baseSW = texture2D(uSceneSampler, oUv0 + vec2(-TexelSize.x, TexelSize.y)).rgb;
    vec3 baseSE = texture2D(uSceneSampler, oUv0 + TexelSize).rgb;
    float monocol = dot(basecol, gray);
    float monoNW = dot(baseNW, gray);
    float monoNE = dot(baseNE, gray);
    float monoSW = dot(baseSW, gray);
    float monoSE = dot(baseSE, gray);
    float monomin = min(monocol, min(min(monoNW, monoNE), min(monoSW, monoSE)));
    float monomax = max(monocol, max(max(monoNW, monoNE), max(monoSW, monoSE)));
    vec2 dir = vec2(-((monoNW + monoNE) - (monoSW + monoSE)), ((monoNW + monoSW) - (monoNE + monoSE)));
    float dirreduce = max((monoNW + monoNE + monoSW + monoSE) * reducemul * 0.25, reducemin);
    float dirmin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirreduce);
    dir = min(vec2(uFXAAStrength), max(vec2(-uFXAAStrength), dir * dirmin)) * TexelSize;

    vec3 resultA = 0.5 * (texture2D(uSceneSampler, oUv0 + dir * -0.166667).rgb + texture2D(uSceneSampler, oUv0 + dir * 0.166667).rgb);
    vec3 resultB = resultA * 0.5 + 0.25 * (texture2D(uSceneSampler, oUv0 + dir * -0.5).rgb + texture2D(uSceneSampler, oUv0 + dir * 0.5).rgb);
    float monoB = dot(resultB, gray);

    if(monoB < monomin || monoB > monomax)
      scene = resultA;
    else
      scene = resultB;
  }
#endif // FXAA
#ifndef NO_MRT
#ifdef MOTION_BLUR
  if (uBlurEnable > 0.0)
  {
    vec2 velocity = uScale * texture2D(uSpeedSampler, oUv0).rg;
    float speed = length(velocity * PixelSize);
    int nSamples = int(clamp(speed, 1.0, float(MAX_SAMPLES)));
    for (int i = 1; i < nSamples; i++) {
      vec2 offset = velocity * (float(i) / float(nSamples - 1) - 0.5);
      vec2 uv = oUv0 + offset;
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
