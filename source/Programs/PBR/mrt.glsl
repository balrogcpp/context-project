// created by Andrey Vasiliev

#ifndef MRT_HELPER_GLSL
#define MRT_HELPER_GLSL
#ifdef OGRE_FRAGMENT_SHADER

uniform float FarClipDistance;
uniform float NearClipDistance;

#ifdef FORCE_TONEMAP
#include "tonemap.glsl"
#endif

void EvaluateBuffer(const vec3 color)
{
#ifdef FORCE_TONEMAP
    FragColor.rgb = SafeHDR(unreal(color));
#else
    FragColor.rgb = SafeHDR(color);
#endif

#ifdef HAS_MRT
    FragData[MRT_DEPTH].r = (gl_FragCoord.z / gl_FragCoord.w - NearClipDistance) / (FarClipDistance - NearClipDistance);
#endif
}

void EvaluateBuffer(const vec3 color, float alpha)
{
    EvaluateBuffer(color);
    FragColor.a = alpha;
}

#endif // OGRE_FRAGMENT_SHADER
#endif // MRT_HELPER_GLSL
