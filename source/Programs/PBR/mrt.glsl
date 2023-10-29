// created by Andrey Vasiliev

#ifndef MRT_HELPER_GLSL
#define MRT_HELPER_GLSL
#ifdef OGRE_FRAGMENT_SHADER

uniform float FarClipDistance;
uniform float NearClipDistance;

#ifdef FORCE_TONEMAP
#include "tonemap.glsl"
#endif

void EvaluateBuffer(vec3 color)
{
#ifdef FORCE_TONEMAP
    color = unreal(expose(color, 2.5));
#endif

    FragColor.rgb = SafeHDR(color);
#ifdef HAS_MRT
    FragData[MRT_DEPTH].r = (gl_FragCoord.z / gl_FragCoord.w - NearClipDistance) / (FarClipDistance - NearClipDistance);
    FragData[MRT_VELOCITY].rg = vec2(0.0, 0.0);
    FragData[MRT_GLOSS].rgb = vec3(0.0, 0.0, 0.0);
    FragData[MRT_NORMALS].rgb = vec3(0.0, 0.0, 0.0);
#endif
}

void EvaluateBuffer(vec3 color, float alpha)
{
    EvaluateBuffer(color);
    FragColor.a = alpha;
}

#endif // OGRE_FRAGMENT_SHADER
#endif // MRT_HELPER_GLSL
