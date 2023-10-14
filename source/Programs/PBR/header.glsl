// created by Andrey Vasiliev

#ifndef HEADER_GLSL
#define HEADER_GLSL

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 8
#endif
#ifndef MAX_SHADOW_TEXTURES
#define MAX_SHADOW_TEXTURES 8
#endif
#ifndef MAX_MRT_TEXTURES
#define MAX_MRT_TEXTURES 5
#endif

#ifdef OGRE_GLSLES
#if __VERSION__ < 300
#extension GL_OES_standard_derivatives : enable
#endif
precision highp float;
precision highp int;
precision lowp sampler2D;
precision lowp samplerCube;
#endif


#if (defined(OGRE_GLSL) && __VERSION__ >= 150) || (defined(OGRE_GLSLES) && __VERSION__ >= 300)
#ifdef OGRE_VERTEX_SHADER
#define attribute in
#define varying out
#endif
#ifdef OGRE_FRAGMENT_SHADER
#define varying in
#ifdef HAS_MRT
out vec4 FragData[MAX_MRT_TEXTURES];
#else
out vec4 FragColor;
#endif
#endif
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
#endif

#if (defined(OGRE_GLSL) && __VERSION__ < 150) || (defined(OGRE_GLSLES) && __VERSION__ < 300)
#ifdef OGRE_FRAGMENT_SHADER
#define FragData gl_FragData
#define FragColor gl_FragColor
#endif
#endif

#ifdef HAS_MRT
#define MRT_COLOR 0
#define MRT_DEPTH 1
#define MRT_VELOCITY 2
#define MRT_NORMALS 3
#define MRT_GLOSS 4
#endif

#if defined(OGRE_GLSL) || defined(OGRE_GLSLES)
#include "hlsl2.glsl"
#endif
#ifdef OGRE_HLSL
#include "glsl2.hlsl"
#include "HLSL_SM4Support.hlsl"
#endif
#include "math.glsl"
#ifdef CHECKERBOARD
#include "mosaic.glsl"
#endif

#endif // HEADER_GLSL
