// This source file is part of Glue Engine. Created by Andrey Vasiliev

#ifndef GL_ES
#version 120
#define VERSION 120
#else
#version 100
#define VERSION 100
#endif

#include "header.frag"

#ifndef MAX_SAMPLES
#define MAX_SAMPLES 8
#endif

in vec2 oUv0;
uniform sampler2D uSceneSampler;

#ifndef NO_MRT
#include "srgb.glsl"
#include "fog.glsl"

uniform vec2 TexelSize;
#ifndef MOTION_BLUR
uniform sampler2D uSceneDepthSampler;
#else
uniform sampler2D uSpeedSampler;
#endif // ! MOTION_BLUR
#ifdef SSAO
uniform sampler2D SsaoSampler;
uniform float uSSAOEnable;
#endif // SSAO
#ifdef BLOOM
#define KERNEL_SIZE 9
uniform sampler2D BloomSampler;
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
uniform float uScale;
uniform float uMotionBlurEnable;
#endif // MOTION_BLUR
#endif // !NO_MRT

void main()
{
  vec3 scene = texture2D(uSceneSampler, oUv0).rgb;

#ifndef NO_MRT
#ifdef SSAO
  if (uSSAOEnable > 0.0) {
    float color = 0.0;
    for (int x = -2; x < 1; x++)
    for (int y = -2; y < 1; y++)
      color += texture2D(SsaoSampler, vec2(oUv0.x + float(x) * TexelSize.x, oUv0.y + float(y) * TexelSize.y)).r;
    color /= 9.0;
    scene.rgb *= vec3(clamp(color + 0.25, 0.0, 1.0));
  }
#endif
#ifdef BLOOM
  if (uBloomEnable > 0.0) {
    float weights0 = 1.0/16.0;
    float weights1 = 2.0/16.0;
    float weights2 = 1.0/16.0;
    float weights3 = 2.0/16.0;
    float weights4 = 4.0/16.0;
    float weights5 = 2.0/16.0;
    float weights6 = 1.0/16.0;
    float weights7 = 2.0/16.0;
    float weights8 = 1.0/16.0;

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

    color += (weights0 * texture2D(BloomSampler, oUv0 + offsets0).rgb);
    color += (weights1 * texture2D(BloomSampler, oUv0 + offsets1).rgb);
    color += (weights2 * texture2D(BloomSampler, oUv0 + offsets2).rgb);
    color += (weights3 * texture2D(BloomSampler, oUv0 + offsets3).rgb);
    color += (weights4 * texture2D(BloomSampler, oUv0 + offsets4).rgb);
    color += (weights5 * texture2D(BloomSampler, oUv0 + offsets5).rgb);
    color += (weights6 * texture2D(BloomSampler, oUv0 + offsets6).rgb);
    color += (weights7 * texture2D(BloomSampler, oUv0 + offsets7).rgb);
    color += (weights8 * texture2D(BloomSampler, oUv0 + offsets8).rgb);

    scene.rgb += vec3(0.65 * color);
  }
#endif
#ifdef FOG
  {
    float clampedDepth = texture2D(uSceneDepthSampler, oUv0).r;
    float fragmentWorldDepth = clampedDepth * FarClipDistance;
    scene = ApplyFog(scene, FogParams, FogColour, fragmentWorldDepth);
  }
#endif
#ifdef MOTION_BLUR
  if (uMotionBlurEnable > 0.0) {
    vec2 velocity = uScale * texture2D(uSpeedSampler, oUv0).rg;
    float speed = length(velocity / TexelSize);
    int nSamples = int(clamp(speed, 1.0, float(MAX_SAMPLES)));
    for (int i = 1; i < nSamples; i++) {
      vec2 offset = velocity * (float(i) / float(nSamples - 1) - 0.5);
      vec2 uv = oUv0 + offset;
      scene += texture2D(uSceneSampler, uv).rgb;
    }
    scene /= float(nSamples);
  }
#endif
#ifdef MANUAL_SRGB
#ifdef SRGB
  scene.rgb = LINEARtoSRGB(scene.rgb, uExposure);
#endif
#endif
#endif //!NO_MRT
  FragColor.rgb = scene;
}
