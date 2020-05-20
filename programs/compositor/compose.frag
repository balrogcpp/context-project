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

uniform sampler2D ShadowSampler0;
uniform sampler2D ShadowSampler1;
uniform sampler2D ShadowSampler2;

void main()
{
  gl_FragColor = vec4(texture2D(ShadowSampler0, oUv0).r, texture2D(ShadowSampler1, oUv0).r, texture2D(ShadowSampler2, oUv0).r, 1.0);
}