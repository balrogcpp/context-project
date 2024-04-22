// created by Andrey Vasiliev

#ifndef MRT_HELPER_GLSL
#define MRT_HELPER_GLSL
#ifdef OGRE_FRAGMENT_SHADER

// uniform float FarClipDistance;
// uniform float NearClipDistance;

#ifdef FORCE_TONEMAP
#include "tonemap.glsl"
#endif



// void EvaluateBuffer(const vec3 color)
// {
// #ifdef FORCE_TONEMAP
//     FragColor.rgb = SafeHDR(unreal(color));
// #else 
//     FragColor.rgb = SafeHDR(color);
// #endif
// }

// void EvaluateBuffer(const vec4 color)
// {
// #ifdef FORCE_TONEMAP
//     FragColor = SafeHDR(unreal(color));
// #else 
//     FragColor = SafeHDR(color);
// #endif
// }

// void EvaluateBuffer(const vec3 color, float depth)
// {
// #ifdef FORCE_TONEMAP
//     FragColor.rgb = SafeHDR(unreal(color));
// #else 
//     FragColor.rgb = SafeHDR(color);
// #endif

// #ifdef HAS_MRT
//     FragData[MRT_DEPTH].x = SafeHDR(depth);
// #endif
// }

#endif // OGRE_FRAGMENT_SHADER
#endif // MRT_HELPER_GLSL
