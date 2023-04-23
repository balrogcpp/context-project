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
#define MAX_MRT_TEXTURES 6
#endif

#ifdef OGRE_GLSLES
#define OGRE_GLSL
#endif

#ifdef OGRE_GLSLES
#if __VERSION__ < 300
#extension GL_OES_standard_derivatives : enable
#endif
#endif


// Vertex shader
#ifdef OGRE_VERTEX_SHADER
#ifdef OGRE_GLSL

#ifndef GL_ES
#if __VERSION__ >= 150
#define attribute in
#define varying out
#else // __VERSION__ < 150
#define in attribute
#define out varying
#endif // __VERSION__ > 150

#else // #ifdef GL_ES
precision highp float;
precision highp int;
precision lowp sampler2D;
precision lowp samplerCube;
#if __VERSION__ >= 300
#define attribute in
#define varying out
#else // __VERSION__ < 300
#define in attribute
#define out varying
#endif // __VERSION__ >= 300
#endif // GLSL_ES

#endif // OGRE_GLSL
#endif // OGRE_VERTEX_SHADER


// Fragment shader
#ifdef OGRE_FRAGMENT_SHADER
#ifdef OGRE_GLSL

#ifndef GL_ES
#if __VERSION__ >= 150
#define varying in
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
#ifdef USE_MRT
out vec4 FragData[MAX_MRT_TEXTURES];
#else // NO_MRT
#define NO_MRT
out vec4 FragColor;
#endif // ! NO_MRT
#else  // #if __VERSION__ < 150
#define in varying
#define out varying
#define FragData gl_FragData
#define FragColor gl_FragColor
#endif // __VERSION__ >= 150

#else // #ifdef GL_ES
precision highp float;
precision highp int;
precision lowp sampler2D;
precision lowp samplerCube;
#if __VERSION__ >= 300
#define varying in
#define texture1D texture
#define texture2D texture
#define texture2DProj textureProj
#define shadow2DProj textureProj
#define texture3D texture
#define textureCube texture
#define texture2DLod textureLod
#define textureCubeLod textureLod
#ifdef USE_MRT
out vec4 FragData[MAX_MRT_TEXTURES];
#else // NO_MRT
#define NO_MRT
out vec4 FragColor;
#endif // !NO_MRT
#else // __VERSION__ < 300
#define in varying
#define out varying
#define FragData gl_FragData
#define FragColor gl_FragColor
#endif // __VERSION__ >= 300
#endif // GLSL_ES

#endif // OGRE_GLSL
#endif // OGRE_FRAGMENT_SHADER


#ifdef OGRE_GLSL
#include "hlsl2.glsl"

#define OGRE_UNIFORMS_BEGIN
#define OGRE_UNIFORMS_END

#if __VERSION__ >= 420
#define _UNIFORM_BINDING(b) layout(binding = b) uniform
#else
#define _UNIFORM_BINDING(b) uniform
#endif // __VERSION__ >= 420

#define SAMPLER1D(name, reg) _UNIFORM_BINDING(reg) sampler1D name
#define SAMPLER2D(name, reg) _UNIFORM_BINDING(reg) sampler2D name
#define SAMPLER3D(name, reg) _UNIFORM_BINDING(reg) sampler3D name
#define SAMPLER2DARRAY(name, reg) _UNIFORM_BINDING(reg) sampler2DArray name
#define SAMPLERCUBE(name, reg) _UNIFORM_BINDING(reg) samplerCube name
#define SAMPLER2DSHADOW(name, reg) _UNIFORM_BINDING(reg) sampler2D name

#if defined(OGRE_GLSLANG) || (__VERSION__ > 150 && defined(OGRE_VERTEX_SHADER)) || __VERSION__ >= 410
#define IN(decl, loc) layout(location = loc) in decl;
#else
#define IN(decl, loc) in decl;
#endif // defined(OGRE_GLSLANG) || (__VERSION__ > 150 && defined(OGRE_VERTEX_SHADER)) || __VERSION__ >= 410

#if defined(OGRE_GLSLANG) || (__VERSION__ > 150 && defined(OGRE_FRAGMENT_SHADER)) || __VERSION__ >= 410
#define OUT(decl, loc) layout(location = loc) out decl;
#else
#define OUT(decl, loc) out decl;
#endif // defined(OGRE_GLSLANG) || (__VERSION__ > 150 && defined(OGRE_FRAGMENT_SHADER)) || __VERSION__ >= 410

#endif // OGRE_GLSL


#ifdef OGRE_HLSL
#include "glsl2.hlsl"
#include "HLSL_SM4Support.hlsl"

#define OGRE_UNIFORMS_BEGIN
#define OGRE_UNIFORMS_END

#define MAIN_PARAMETERS void main(

#ifdef OGRE_VERTEX_SHADER
#define MAIN_DECLARATION out float4 gl_Position : POSITION)
#else
#define MAIN_DECLARATION in float4 gl_FragCoord : POSITION, out float4 FragColor : COLOR)
#endif // OGRE_VERTEX_SHADER

#define IN(decl, sem) in decl : sem,
#define OUT(decl, sem) out decl : sem,

#endif // OGRE_HLSL


#if !defined(OGRE_HLSL) && !defined(OGRE_CG)
// semantics as aliases for attribute locations
#define POSITION    0
#define BLENDWEIGHT 1
#define NORMAL      2
#define COLOR0      3
#define COLOR1      4
#define COLOR COLOR0
#define FOG         5
#define BLENDINDICES 7
#define TEXCOORD0   8
#define TEXCOORD1   9
#define TEXCOORD2  10
#define TEXCOORD3  11
#define TEXCOORD4  12
#define TEXCOORD5  13
#define TEXCOORD6  14
#define TEXCOORD7  15
#define TANGENT    14
#endif // !defined(OGRE_HLSL) && !defined(OGRE_CG)


#endif // HEADER_GLSL
