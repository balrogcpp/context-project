// created by Andrey Vasiliev

#ifndef MRT_HELPER_GLSL
#define MRT_HELPER_GLSL
#ifdef OGRE_FRAGMENT_SHADER

uniform float FarClipDistance;
uniform float NearClipDistance;

#ifdef HAS_TONEMAP
#include "tonemap.glsl"
#endif

void EvaluateBuffer(vec3 color, const float alpha)
{
#ifdef HAS_TONEMAP
    color = unreal(expose(color, 8.0));
#endif
#ifdef HAS_MRT
    FragData[MRT_COLOR] = vec4(SafeHDR(color), alpha);
    FragData[MRT_DEPTH].r = (gl_FragCoord.z / gl_FragCoord.w - NearClipDistance) / (FarClipDistance - NearClipDistance);
    FragData[MRT_VELOCITY].rg = vec2(0.0, 0.0);
    FragData[MRT_GLOSS].rgb = vec3(0.0, 0.0, 0.0);
    FragData[MRT_NORMALS].rgb = vec3(0.0, 0.0, 0.0);
#else
    FragColor = vec4(SafeHDR(color), alpha);
#endif
}

void EvaluateBuffer(vec3 color)
{
#ifdef HAS_TONEMAP
    color = unreal(expose(color, 8.0));
#endif
#ifdef HAS_MRT
    FragData[MRT_COLOR].rgb = SafeHDR(color);
    FragData[MRT_DEPTH].r = (gl_FragCoord.z / gl_FragCoord.w - NearClipDistance) / (FarClipDistance - NearClipDistance);
    FragData[MRT_VELOCITY].rg = vec2(0.0, 0.0);
    FragData[MRT_GLOSS].rgb = vec3(0.0, 0.0, 0.0);
    FragData[MRT_NORMALS].rgb = vec3(0.0, 0.0, 0.0);
#else
    FragColor.rgb = SafeHDR(color);
#endif
}

#endif // OGRE_FRAGMENT_SHADER
#endif // MRT_HELPER_GLSL
