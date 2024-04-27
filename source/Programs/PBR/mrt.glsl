// created by Andrey Vasiliev

#ifndef MRT_HELPER_GLSL
#define MRT_HELPER_GLSL
#ifdef OGRE_FRAGMENT_SHADER

#ifdef FORCE_TONEMAP
#include "tonemap.glsl"
#endif

vec2 encode(const vec3 n)
{
    float p = sqrt(n.z * 8.0 + 8.0);
    return vec2(vec2(n.x, n.y) / p + 0.5);
}

vec3 decode(const vec2 enc)
{
    vec2 fenc = enc * 4.0 - 2.0;
    float f = dot(fenc, fenc);
    float g = sqrt(1.0 - f / 4.0);
    return vec3(fenc * g, 1.0 - f / 2.0);
}

void EvaluateBuffer(const vec3 color)
{
#ifdef FORCE_TONEMAP
    FragColor.rgb = SafeHDR(unreal(color));
#else
    FragColor.rgb = SafeHDR(color);
#endif
}

void EvaluateBuffer(const vec4 color, float depth, const vec3 normal, const vec2 velocity, float roughness)
{
#ifdef FORCE_TONEMAP
    FragColor = SafeHDR(vec4(unreal(color.rgb), color.a));
#else
    FragColor = SafeHDR(color);
#endif

#ifdef HAS_MRT
    FragData[MRT_DEPTH].rgb = vec3(depth, encode(normal));
    FragData[MRT_VELOCITY].rgb = vec3(velocity, roughness);
#endif
}

#endif // OGRE_FRAGMENT_SHADER
#endif // MRT_HELPER_GLSL
