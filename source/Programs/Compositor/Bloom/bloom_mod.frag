// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif

#include "header.frag"
#include "filters.glsl"
#ifdef USE_HDR
#include "tonemap.glsl"
#endif // USE_HDR


in vec2 vUV0;
uniform sampler2D uRT;
uniform sampler2D uRT0;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 rt = texture2D(uRT, vUV0).rgb;
  vec3 rt0 = texture2D(uRT0, vUV0).rgb;

#ifdef USE_HDR
  {
    float lum = dot(K, vec3(0.27, 0.67, 0.06));
    scene = tone_map(scene, lum);
  }
#endif // USE_HDR

  FragColor.rgb = rt + rt0 * (1.0 / 7.0);
}
