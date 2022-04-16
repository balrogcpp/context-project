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
uniform sampler2D SceneSampler;

#ifndef NO_MRT
#include "srgb.glsl"
#include "fog.glsl"

uniform vec2 texelSize;
#ifndef MOTION_BLUR
uniform sampler2D sSceneDepthSampler;
#else
uniform sampler2D SpeedSampler;
#endif // ! MOTION_BLUR
#ifdef SSAO
uniform sampler2D SsaoSampler;
uniform float uSSAOEnable;
#endif // SSAO
#ifdef BLOOM
uniform sampler2D BloomSampler;
uniform float uBloomEnable;
#endif // BLOOM
#ifdef SRGB
#ifdef MANUAL_SRGB
uniform float exposure;
#endif // MANUAL_SRGB
#endif // SRGB
#ifdef FOG
uniform vec3 uFogColour;
uniform vec4 uFogParams;
uniform float nearClipDistance;
uniform float farClipDistance;
#endif // FOG
#ifdef MOTION_BLUR
uniform float uScale;
uniform float uMotionBlurEnable;
#endif // MOTION_BLUR
#endif // !NO_MRT

void main()
{
  vec3 scene = texture2D(SceneSampler, oUv0).rgb;

#ifndef NO_MRT
#ifdef SSAO
  if (uSSAOEnable > 0.0) {
    float color = 0.0;
    for (int x = -2; x < 1; x++)
    for (int y = -2; y < 1; y++)
      color += texture2D(SsaoSampler, vec2(oUv0.x + float(x) * texelSize.x, oUv0.y + float(y) * texelSize.y)).r;
    color /= 9.0;
    scene.rgb *= clamp(color + 0.25, 0.0, 1.0);
  }
#endif
#ifdef BLOOM
  if (uBloomEnable > 0.0) scene.rgb += (0.65 * texture2D(BloomSampler, oUv0).rgb);
#endif
#ifdef FOG
  {
    float clampedDepth = texture2D(sSceneDepthSampler, oUv0).r;
    float fragmentWorldDepth = clampedDepth * farClipDistance;
    scene = ApplyFog(scene, uFogParams, uFogColour, fragmentWorldDepth);
  }
#endif
#ifdef MOTION_BLUR
  if (uMotionBlurEnable > 0.0) {
  vec2 velocity = uScale * texture2D(SpeedSampler, oUv0).rg;
  float speed = length(velocity / texelSize);
  int nSamples = int(clamp(speed, 1.0, float(MAX_SAMPLES)));
  for (int i = 1; i < nSamples; i++) {
    vec2 offset = velocity * (float(i) / float(nSamples - 1) - 0.5);
    vec2 uv = oUv0 + offset;
    scene += texture2D(SceneSampler, uv).rgb;
  }
  scene /= float(nSamples);
}
#endif
#ifdef MANUAL_SRGB
#ifdef SRGB
  scene.rgb = LINEARtoSRGB(scene.rgb, exposure);
#endif
#endif
#endif //!NO_MRT
  FragColor.rgb = scene;
}
