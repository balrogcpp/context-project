// created by Andrey Vasiliev

#ifndef HLSL2_GLSL
#define HLSL2_GLSL
#ifdef OGRE_HLSL


// some glsl functions as macros
#define vec4 float4
#define vec3 float3
#define vec2 float2
#define ivec4 int4
#define ivec3 int3
#define ivec2 int2
#define uvec4 uint4
#define uvec3 uint3
#define uvec2 uint2
#define bvec4 bool4
#define bvec3 bool3
#define bvec2 bool2
#define mat2x2 float2x2
#define mat3x2 float2x3
#define mat4x2 float2x4
#define mat2x3 float3x2
#define mat3x3 float3x3
#define mat4x3 float3x4
#define mat2x4 float4x2
#define mat3x4 float4x3
#define mat4x4 float4x4
#define mat4x4 matrix
#define precision
#define highp
#define mediump
#define lowp
#define dFdx ddx
#define dFdy ddy
#define dFdx ddx_coarse
#define dFdy ddy_coarse
#define dFdx ddx_fine
#define dFdy ddy_fine
#define mix lerp
#define modf fmod
#define fract frac
#define atan atan2
#define inversesqrt rsqrt
#define mod fmod
#define distance dst
#define bitCount countbits
#define findMSB firstbithigh
#define findLSB firstbitlow
#define bitfieldReverse reversebits
#define sampler2D Texture2D
#define sampler3D Texture3D
#define samplerCube TextureCube
#define texture2D tex2D
#define texture3D tex3D
#define textureCube texCube


// from https://github.com/OGRECave/ogre/blob/v13.6.4/Media/Main/OgreUnifiedShader.h
// #define mod(a, b)   ( a - b * floor(a / b) )
#define vec2_splat(x) ( vec2(x, x) )
#define vec3_splat(x) ( vec3(x, x, x) )
#define vec4_splat(x) ( vec4(x, x, x, x) )
#define mtxFromRows3x3(a, b, c)    ( transpose(mat3(a, b, c)) )
#define mtxFromRows4x4(a, b, c, d) ( transpose(mat4(a, b, c, d)) )
#define mtxFromCols3x3(a, b, c)    ( mat3(a, b, c) )
#define mtxFromCols4x4(a, b, c, d) ( mat4(a, b, c, d) )
#define mtx3x3(a1, a2, a3, a4, a5, a6, a7, a8, a9) ( transpose(mat3(a1, a2, a3, a4, a5, a6, a7, a8, a9)) )
#define mtx4x4(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) ( transpose(mat4(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16)) )


// semantics as aliases for attribute locations
#ifdef OGRE_VERTEX_SHADER
#if !defined(OGRE_HLSL) && !defined(OGRE_CG)
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
#endif
#endif


#define OGRE_UNIFORMS_BEGIN
#define OGRE_UNIFORMS_END

#define MAIN_PARAMETERS void main(

#ifdef OGRE_VERTEX_SHADER
#define MAIN_DECLARATION out float4 gl_Position : POSITION)
#endif
#ifdef OGRE_FRAGMENT_SHADER
#define MAIN_DECLARATION in float4 gl_FragCoord : POSITION, out float4 FragColor : COLOR)
#endif

#define IN(decl, sem) in decl : sem,
#define OUT(decl, sem) out decl : sem,


#endif // OGRE_HLSL
#endif // HLSL2_GLSL
