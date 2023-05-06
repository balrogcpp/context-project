// created by Andrey Vasiliev

#ifndef HLSL2_GLSL
#define HLSL2_GLSL
#ifdef OGRE_GLSL


// from https://github.com/OGRECave/ogre/blob/v13.6.4/Media/Main/GLSL_GL3Support.glsl
#if __VERSION__ == 100
mat2 transpose(mat2 m)
{
  return mat2(m[0][0], m[1][0],
              m[0][1], m[1][1]);
}

mat3 transpose(mat3 m)
{
  return mat3(m[0][0], m[1][0], m[2][0],
              m[0][1], m[1][1], m[2][1],
              m[0][2], m[1][2], m[2][2]);
}

mat4 transpose(mat4 m)
{
  return mat4(m[0][0], m[1][0], m[2][0], m[3][0],
              m[0][1], m[1][1], m[2][1], m[3][1],
              m[0][2], m[1][2], m[2][2], m[3][2],
              m[0][3], m[1][3], m[2][3], m[3][3]);
}
#endif // __VERSION__ == 100


// some hlsl functions as macros
#define float4 vec4
#define float3 vec3
#define float2 vec2
#define int4 ivec4
#define int3 ivec3
#define int2 ivec2
#define uint4 uvec4
#define uint3 uvec3
#define uint2 uvec2
#define bool4 bvec4
#define bool3 bvec3
#define bool2 bvec2
#define float2x2 mat2x2
#define float2x3 mat3x2
#define float2x4 mat4x2
#define float3x2 mat2x3
#define float3x3 mat3x3
#define float3x4 mat4x3
#define float4x2 mat2x4
#define float4x3 mat3x4
#define float4x4 mat4x4
#define matrix mat4x4
#define static
#define groupshared shared
#define ddx dFdx
#define ddy dFdy
#define ddx_coarse dFdx
#define ddy_coarse dFdy
#define ddx_fine dFdx
#define ddy_fine dFdy
#ifdef GL_ES
#   define mad(a, b, c) ((a) * (b) + (c))
#else
#   define mad fma
#endif
#define mul(a, b) ((a) * (b))
#define round(a) floor(a + 0.5)
#define saturate(x) clamp(x, 0.0, 1.0)
#define rcp(x) (1.0 / x)
#define frac fract
#define atan2 atan
#define rsqrt inversesqrt
#define fmod mod
#define lerp mix
#define dst distance
#define countbits bitCount
#define firstbithigh findMSB
#define firstbitlow findLSB
#define reversebits bitfieldReverse
#define Texture2D sampler2D
#define Texture3D sampler3D
#define TextureCube samplerCube
#define tex2D texture2D
#define tex3D texture3D
#define texCube textureCube


// from https://github.com/OGRECave/ogre/blob/v13.6.4/Media/Main/OgreUnifiedShader.h
#define vec2_splat vec2
#define vec3_splat vec3
#define vec4_splat vec4
#define mtxFromRows3x3(a, b, c)    ( transpose(mat3(a, b, c)) )
#define mtxFromRows4x4(a, b, c, d) ( transpose(mat4(a, b, c, d)) )
#define mtxFromCols3x3(a, b, c)    ( mat3(a, b, c) )
#define mtxFromCols4x4(a, b, c, d) ( mat4(a, b, c, d) )
#define mtx3x3(a1, a2, a3, a4, a5, a6, a7, a8, a9) ( mat3(a1, a2, a3, a4, a5, a6, a7, a8, a9) )
#define mtx4x4(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) ( mat4(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16) )


#endif // OGRE_GLSL
#endif // HLSL2_GLSL
