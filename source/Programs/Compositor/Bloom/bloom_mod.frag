// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define VERSION 150
#else
#version 100
#define VERSION 100
#endif

#include "header.frag"
#include "filters.glsl"
#ifdef USE_HDR
#include "tonemap.glsl"
#endif // USE_HDR


in vec2 vUV0;
uniform sampler2D uSampler;
uniform sampler2D uRT1;
uniform sampler2D uRT2;
uniform sampler2D uRT3;
uniform sampler2D uRT4;
uniform sampler2D uRT5;
uniform sampler2D uRT6;
uniform sampler2D uRT7;
uniform sampler2D uRT8;
uniform sampler2D uRT9;
uniform sampler2D uRT10;
uniform vec2 TexelSize0;
uniform vec2 TexelSize1;
uniform vec2 TexelSize2;
uniform vec2 TexelSize3;
uniform vec2 TexelSize4;
uniform vec2 TexelSize5;
uniform vec2 TexelSize6;
uniform vec2 TexelSize7;
uniform vec2 TexelSize8;
uniform vec2 TexelSize9;
uniform vec2 TexelSize10;
uniform float uThreshhold;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
  vec3 scene = texture2D(uSampler, vUV0).rgb;

  vec3 A = Linear(uRT1, vUV0, TexelSize1);
  vec3 B = Linear(uRT2, vUV0, TexelSize2);
  vec3 C = Upscale3x3(uRT3, vUV0, TexelSize3);
  vec3 D = Upscale3x3(uRT4, vUV0, TexelSize4);
  vec3 E = Upscale3x3(uRT5, vUV0, TexelSize5);
  vec3 F = Upscale3x3(uRT6, vUV0, TexelSize6);
  vec3 G = Upscale3x3(uRT7, vUV0, TexelSize7);
  vec3 H = Upscale3x3(uRT8, vUV0, TexelSize8);
  vec3 I = Upscale3x3(uRT9, vUV0, TexelSize9);
  vec3 K = Upscale3x3(uRT10, vUV0, TexelSize10);

  scene += (A + B + C + D) * 0.65;
  scene += (E + F + G + H) * 0.25;
  scene += (I + K) * 0.25;
#ifdef USE_HDR
  {
    float lum = dot(K, vec3(0.27, 0.67, 0.06));
    scene = tone_map(scene, lum);
  }
#endif // USE_HDR

  FragColor.rgb = scene;
}
