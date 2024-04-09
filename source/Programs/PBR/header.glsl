// created by Andrey Vasiliev

#ifndef HEADER_GLSL
#define HEADER_GLSL

//#undef HAS_MRT
#ifdef GL_ES
//#define FORCE_TONEMAP
#endif

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 8
#endif
#ifdef GL_ES
#define MAX_SHADOW_TEXTURES 0
#endif
#define SHADOWMAP_ATLAS
#define PSSM_ESM_SHADOWMAP
#define PSSM_GLOBAL_MIN_DEPTH 1.0
#define PSSM_GLOBAL_RANGE 64.0
#define MRT_COLOR 0
#define MRT_DEPTH 1
#ifndef GL_ES
#define MRT_VELOCITY 2
#endif
#define MRT_NORMALS 3
#define MRT_GLOSS 4
#define MAX_MRT_TEXTURES 5

#ifdef OGRE_GLSLES
#if __VERSION__ < 300
#extension GL_OES_standard_derivatives : enable
#endif
precision mediump float;
precision mediump int;
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
#define FragColor FragData[MRT_COLOR]
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

#ifdef OGRE_FRAGMENT_SHADER
#if (defined(OGRE_GLSL) && __VERSION__ < 150) || (defined(OGRE_GLSLES) && __VERSION__ < 300)
#define FragData gl_FragData
#define FragColor gl_FragColor
#endif
#endif

#if defined(OGRE_GLSL) || defined(OGRE_GLSLES)
#include "hlsl2.glsl"
#elif defined(OGRE_HLSL)
#include "glsl2.hlsl"
#endif
#include "math.glsl"
#include "mrt.glsl"

#endif // HEADER_GLSL
