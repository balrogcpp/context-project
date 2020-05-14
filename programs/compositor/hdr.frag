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
precision highp float;
#define in varying
#define out varying
#endif

#if VERSION != 120
out vec4 gl_FragColor;
#endif


in vec2 oUv0;

uniform float exposure;
uniform vec4 screenSize;
uniform sampler2D SceneSampler;

void main()
{
  vec3 color = texture2D(SceneSampler, oUv0).rgb;

  const float gamma = 2.2;

  // Exposure tone mapping
  vec3 mapped = vec3(1.0) - exp(-color * exposure);
  // Gamma correction
  mapped = pow(mapped, vec3(1.0 / gamma));

  gl_FragColor = vec4(mapped, 1.0);
}