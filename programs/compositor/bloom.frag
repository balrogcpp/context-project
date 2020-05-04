#ifndef GL_ES
#define VERSION 120
#version VERSION
#define USE_TEX_LOD
#if VERSION != 120
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
#else
#define in varying
#define out varying
#endif
#ifdef USE_TEX_LOD
#extension GL_ARB_shader_texture_lod : require
#endif
#else
#version 100
#extension GL_OES_standard_derivatives : enable
#extension GL_EXT_shader_texture_lod: enable
#define textureCubeLod textureLodEXT
precision mediump float;
#define in varying
#define out varying
#endif

#if VERSION != 120
out vec4 gl_FragColor;
#endif

in vec2 oUv0;
uniform vec4 screenSize;
uniform sampler2D SceneSampler;
//uniform sampler2D AttenuationSampler;

void main()
{
  float a = texture2D(SceneSampler, oUv0).a;
  vec3 color = texture2D(SceneSampler, oUv0).rgb;
  vec3 bloom = vec3(0.0);

  const float treshhold = 0.9;

//  bloom.r = color.r > treshhold ? color.r : 0.0;
//  bloom.g = color.g > treshhold ? color.g : 0.0;
//  bloom.b = color.b > treshhold ? color.b : 0.0;
  bloom.r = color.r > treshhold ? color.r - treshhold : 0.0;
  bloom.g = color.g > treshhold ? color.g - treshhold : 0.0;
  bloom.b = color.b > treshhold ? color.b - treshhold : 0.0;

  gl_FragColor = vec4(bloom, a);
}