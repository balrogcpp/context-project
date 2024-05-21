// created by Andrey Vasiliev

//#undef HAS_MRT
//#define FORCE_TONEMAP

#ifndef MAX_LIGHTS
#define MAX_LIGHTS 8
#endif
#ifndef MAX_SHADOW_TEXTURES
#define MAX_SHADOW_TEXTURES 4
#endif
//#ifdef GL_ES
//#undef MAX_SHADOW_TEXTURES
//#define MAX_SHADOW_TEXTURES 0
//#endif
#define SHADOWMAP_ATLAS
#define FORCE_FOG
#define PSSM_SPLITS 2
#define PSSM_ESM_SHADOWMAP
#define PSSM_GLOBAL_MIN_DEPTH 1.0
#define PSSM_GLOBAL_RANGE 64.0
#define MRT_COLOR 0
#define MRT_DEPTH 1
#define MRT_VELOCITY 2
#define MAX_MRT_TEXTURES 3

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
#define saturate(x) clamp(x, 0.0, 1.0)
#define mul(a, b) ((a) * (b))
#define mtxFromCols(a, b, c) mat3(a, b, c)
#else
#define mtxFromCols(a, b, c) transpose(mat3(a, b, c))
//#include "OgreUnifiedShader.h"
#endif
#include "math.glsl"
#ifdef OGRE_FRAGMENT_SHADER
#include "mrt.glsl"
#endif
