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
    FragColor = vec4(SafeHDR(unreal(color)), 1.0);
#else
    FragColor = vec4(SafeHDR(color), 1.0);
#endif

#ifdef HAS_MRT
    FragData[MRT_DEPTH] = vec4((gl_FragCoord.z / gl_FragCoord.w - NearClipDistance) / (FarClipDistance - NearClipDistance), 0.0, 0.0, 1.0);
    FragData[MRT_GLOSS] = vec4(0.0, 0.0, 0.0, 1.0);
    FragData[MRT_VELOCITY] = vec4(0.0, 0.0, 0.0, 1.0);
    FragData[MRT_NORMALS] = vec4(0.0, 0.0, 0.0, 1.0);
#endif
}

void EvaluateBuffer(const vec3 color, float alpha)
{
    EvaluateBuffer(color);
    FragColor.a = alpha;
}

#endif // OGRE_FRAGMENT_SHADER
#endif // MRT_HELPER_GLSL
