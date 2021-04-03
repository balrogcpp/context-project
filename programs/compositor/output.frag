//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

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
#endif

#ifdef MANUAL_SRGB
uniform float exposure;
#endif

uniform vec3 uFogColour;
uniform vec4 uFogParams;
uniform float nearClipDistance;
uniform float farClipDistance;

uniform vec2 texelSize;
uniform float uScale;
uniform float uMotionBlurEnable;

void main()
{
  vec3 scene = texture2D(SceneSampler, oUv0).rgb;
#ifndef GL_ES


#ifdef SSAO
  scene.rgb *= texture2D(SsaoSampler, oUv0).r;
#endif

  float clampedDepth = texture2D(sSceneDepthSampler, oUv0).r;
  float fragmentWorldDepth = clampedDepth * farClipDistance - nearClipDistance;
  scene = ApplyFog(scene, uFogParams, uFogColour, fragmentWorldDepth);

#ifdef MOTION_BLUR
  if (uMotionBlurEnable > 0.0) {
  vec2 velocity = uScale * texture2D(sSceneDepthSampler, oUv0).gb;
  float speed = length(velocity / texelSize);
  int nSamples = int(clamp(speed, 1.0, float(MAX_SAMPLES)));

  for (int i = 1; i < nSamples; i++) {
    vec2 offset = velocity * (float(i) / float(nSamples - 1) - 0.5);
    vec2 uv = oUv0 + offset;

    float clampedDepth = texture2D(sSceneDepthSampler, uv).r;
    float fragmentWorldDepth = clampedDepth * farClipDistance - nearClipDistance;
    scene += ApplyFog(texture2D(SceneSampler, uv).rgb, uFogParams, uFogColour, fragmentWorldDepth);
  }

  scene /= float(nSamples);
}
#endif

#ifdef MANUAL_SRGB
  scene.rgb = LINEARtoSRGB(scene.rgb, exposure);
#endif


#endif
  gl_FragColor = vec4(scene.rgb, 1.0);
}
