// created by Andrey Vasiliev

#ifndef HLSL2_GLSL
#define HLSL2_GLSL
#ifdef OGRE_GLSL


// some hlsl functions as macroses
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
#define atan2 atan
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
#define lerp mix
#define fmod modf
#define mul(a, b) ((a)*(b))
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
#define tex2D texture2D
#define tex3D texture3D
#define texCube textureCube



float rcp(float x)
{
    return 1.0 / x;
}

vec2 rcp(vec2 x)
{
    return vec2(1.0) / x;
}

vec3 rcp(vec3 x)
{
    return vec3(1.0) / x;
}

vec4 rcp(vec4 x)
{
    return vec4(1.0) / x;
}

// https://community.khronos.org/t/saturate/53155
float saturate(float x)
{
    return clamp(x, 0.0, 1.0);
}

vec2 saturate(vec2 x)
{
    return clamp(x, 0.0, 1.0);
}

vec3 saturate(vec3 x)
{
    return clamp(x, 0.0, 1.0);
}

vec4 saturate(vec4 x)
{
    return clamp(x, 0.0, 1.0);
}

#endif // OGRE_GLSL
#endif // HLSL2_GLSL
