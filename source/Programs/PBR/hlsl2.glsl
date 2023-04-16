// created by Andrey Vasiliev

#ifndef HLSL2_GLSL
#define HLSL2_GLSL
#ifdef OGRE_GLSL


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


#endif // OGRE_GLSL
#endif // HLSL2_GLSL
