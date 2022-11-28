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

in vec2 vUV0;
uniform sampler2D uSampler;
uniform vec2 TexelSize;
uniform float uThreshhold;

//----------------------------------------------------------------------------------------------------------------------
vec3 ApplyThreshold(const vec3 color, const float threshold)
{
  // convert rgb to grayscale/brightness
  float brightness = dot(color, vec3(0.2126, 0.7152, 0.0722));
  if (brightness > threshold) {
    return color;
  } else {
    return vec3(0.0);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 color = texture2D(uSampler, vUV0).rgb;
  FragColor.rgb = ApplyThreshold(color, uThreshhold);
}
