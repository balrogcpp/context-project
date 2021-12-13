// This source file is part of "glue project". Created by Andrew Vasiliev

#ifndef GL_ES
#define VERSION 120
#version VERSION
#else
#define VERSION 100
#version VERSION
#endif
#include "header.frag"

#include "srgb.glsl"
#include "fog.glsl"

in vec2 oUv0;
uniform sampler2D SceneSampler;
uniform sampler2D sSceneDepthSampler;

#ifdef SSAO
uniform sampler2D SsaoSampler;
uniform float uSSAOEnable;
#endif

#ifdef BLOOM
uniform sampler2D BloomSampler;
uniform float uBloomEnable;
#endif

#ifdef SRGB
#ifdef MANUAL_SRGB
uniform float exposure;
#endif
#endif

#ifdef FOG
uniform vec3 uFogColour;
uniform vec4 uFogParams;
uniform float nearClipDistance;
uniform float farClipDistance;
#endif
#ifdef MOTION_BLUR
uniform vec2 texelSize;
uniform float uScale;
uniform float uMotionBlurEnable;
#endif

void main()
{
  vec3 scene = texture2D(SceneSampler, oUv0).rgb;


#ifndef GL_ES


#ifdef SSAO
if (uSSAOEnable > 0.0) {
  scene.rgb *= texture2D(SsaoSampler, oUv0).r;
}
#endif

#ifdef BLOOM
if (uBloomEnable > 0.0) {
  scene.rgb += texture2D(BloomSampler, oUv0).rgb;
}
#endif

#ifdef FOG
  float clampedDepth = texture2D(sSceneDepthSampler, oUv0).r;
  float fragmentWorldDepth = clampedDepth * farClipDistance - nearClipDistance;
  scene = ApplyFog(scene, uFogParams, uFogColour, fragmentWorldDepth);
#endif

#ifdef MOTION_BLUR
  if (uMotionBlurEnable > 0.0) {
  vec2 velocity = uScale * texture2D(sSceneDepthSampler, oUv0).gb;
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


#endif //!GL_ES


  gl_FragColor = vec4(scene.rgb, 1.0);
}