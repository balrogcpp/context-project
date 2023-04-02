// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"
#include "math.glsl"
#include "srgb.glsl"


#ifndef MAX_SAMPLES
#define MAX_SAMPLES 5
#endif


uniform sampler2D uColorMap;
uniform sampler2D uVelocitySampler;
uniform vec2 TexSize1;
uniform float uScale;

in vec2 vUV0;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(uColorMap, vUV0).rgb;
  vec2 velocity = uScale * texture2D(uVelocitySampler, vUV0).xy;
  float speed = length(velocity * TexSize1);
  float nSamples = ceil(clamp(speed, 1.0, float(MAX_SAMPLES)));
  float invSamples = 1.0 / nSamples;

  for (int i = 1; i < MAX_SAMPLES; ++i) {
    if (int(nSamples) <= i) break;

    vec2 offset = (float(i) * invSamples - 0.5) * velocity;
    color += texture2D(uColorMap, vUV0 + offset).rgb;
  }

  color *= invSamples;
  FragColor.rgb = color;
}
