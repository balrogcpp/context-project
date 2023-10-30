// created by Andrey Vasiliev

#ifndef HEADER_GLSL
#define HEADER_GLSL

//#undef HAS_MRT
//#define FORCE_TONEMAP
#ifndef MAX_LIGHTS
#define MAX_LIGHTS 8
#endif
#ifndef MAX_SHADOW_TEXTURES
#define MAX_SHADOW_TEXTURES 1
#endif
#ifndef MAX_MRT_TEXTURES
#define MAX_MRT_TEXTURES 5
#endif
#ifndef PSSM_SPLIT_COUNT
#define PSSM_SPLIT_COUNT 1
#endif
#ifdef HAS_MRT
#define MRT_COLOR 0
#define MRT_DEPTH 1
#define MRT_VELOCITY 2
#define MRT_NORMALS 3
#define MRT_GLOSS 4
#endif

#ifdef OGRE_GLSLES
#if __VERSION__ < 300
#extension GL_OES_standard_derivatives : enable
#endif
precision mediump float;
precision mediump int;
precision mediump sampler2D;
precision mediump samplerCube;
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
