// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif

#include "header.frag"
#include "srgb.glsl"

#ifndef MAX_SAMPLES
#define MAX_SAMPLES 4.0
#endif


uniform sampler2D uSceneSampler;
uniform sampler2D uSpeedSampler;
uniform vec2 PixelSize1;
uniform float uScale;

in vec2 vUV0;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 scene = texture2D(uSceneSampler, vUV0).rgb;
  vec2 velocity = uScale * texture2D(uSpeedSampler, vUV0).rg;
  float speed = length(velocity * PixelSize1);
  float nSamples = ceil(clamp(speed, 1.0, MAX_SAMPLES));
  float invSamples = 1.0 / nSamples;
  for (float i = 1.0; i < MAX_SAMPLES; i += 1.0) {
    if (nSamples <= i) break;
    vec2 offset = ((i * invSamples - 0.5) * velocity);
    scene += texture2D(uSceneSampler, vUV0 + offset).rgb;
  }
  scene *= invSamples;
  FragColor.rgb = scene;
}
