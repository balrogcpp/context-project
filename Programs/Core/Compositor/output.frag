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
#include "srgb.glsl"
#include "fog.glsl"
#endif

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
#define KERNEL_SIZE 9
uniform sampler2D uBloomSampler;
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
uniform float uMotionBlurEnable;
#endif // MOTION_BLUR
#endif // !NO_MRT
#ifdef FXAA
uniform float uFXAAStrength;
uniform float uFXAAEnable;
#endif // FXAA

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
    color /= 9.0;
    scene.rgb *= vec3(clamp(color + 0.1, 0.0, 1.0));
  }
#endif // SSAO
#ifdef BLOOM
  if (uBloomEnable > 0.0)
  {
    const float weights0 = 1.0/16.0;
    const float weights1 = 2.0/16.0;
    const float weights2 = 1.0/16.0;
    const float weights3 = 2.0/16.0;
    const float weights4 = 4.0/16.0;
    const float weights5 = 2.0/16.0;
    const float weights6 = 1.0/16.0;
    const float weights7 = 2.0/16.0;
    const float weights8 = 1.0/16.0;

    vec2 offsets0 = vec2(-TexelSize.x, -TexelSize.y);
    vec2 offsets1 = vec2(0.0, -TexelSize.y);
    vec2 offsets2 = vec2(TexelSize.x, -TexelSize.y);
    vec2 offsets3 = vec2(-TexelSize.x, 0.0);
    vec2 offsets4 = vec2(0.0, 0.0);
    vec2 offsets5 = vec2(TexelSize.x, 0.0);
    vec2 offsets6 = vec2(-TexelSize.x, TexelSize.y);
    vec2 offsets7 = vec2(0.0,  TexelSize.y);
    vec2 offsets8 = vec2(TexelSize.x, TexelSize.y);

    vec3 color = vec3(0.0);

    color += vec3(weights0 * texture2D(uBloomSampler, oUv0 + offsets0).rgb);
    color += vec3(weights1 * texture2D(uBloomSampler, oUv0 + offsets1).rgb);
    color += vec3(weights2 * texture2D(uBloomSampler, oUv0 + offsets2).rgb);
    color += vec3(weights3 * texture2D(uBloomSampler, oUv0 + offsets3).rgb);
    color += vec3(weights4 * texture2D(uBloomSampler, oUv0 + offsets4).rgb);
    color += vec3(weights5 * texture2D(uBloomSampler, oUv0 + offsets5).rgb);
    color += vec3(weights6 * texture2D(uBloomSampler, oUv0 + offsets6).rgb);
    color += vec3(weights7 * texture2D(uBloomSampler, oUv0 + offsets7).rgb);
    color += vec3(weights8 * texture2D(uBloomSampler, oUv0 + offsets8).rgb);

    scene.rgb += vec3(0.65 * color);
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
#ifdef FXAA
  if (uFXAAEnable > 0.0)
  {
    const float reducemul = 1.0 / 8.0;
    const float reducemin = 1.0 / 128.0;
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
    float monoB = dot(resultB.rgb, gray);

    if(monoB < monomin || monoB > monomax)
      scene = resultA;
    else
      scene = resultB;
  }
#endif // FXAA
#ifndef NO_MRT
#ifdef MOTION_BLUR
  if (uMotionBlurEnable > 0.0)
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
  scene.rgb = LINEARtoSRGB(scene.rgb, uExposure);
#endif // SRGB
#endif // MANUAL_SRGB
  FragColor.rgb = scene;
}
