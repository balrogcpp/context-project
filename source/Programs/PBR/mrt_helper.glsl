// created by Andrey Vasiliev

#ifndef MRT_HELPER_GLSL
#define MRT_HELPER_GLSL
#ifdef OGRE_FRAGMENT_SHADER

#ifdef HAS_MRT
uniform highp float FarClipDistance;
uniform highp float NearClipDistance;
#else
#include "srgb.glsl"
#endif

void EvaluateBuffer(const mediump vec3 color, const mediump float alpha)
{
#ifdef HAS_MRT
    FragColor = vec4(color, alpha);
    FragData[MRT_DEPTH] = vec4((gl_FragCoord.z / gl_FragCoord.w - NearClipDistance) / (FarClipDistance - NearClipDistance), 0.0, 0.0, 0.0);
#else
    FragColor = vec4(LINEARtoSRGB(color), alpha);
#endif
}

void EvaluateBuffer(const mediump vec4 color)
{
#ifdef HAS_MRT
    FragColor = vec4(color);
    FragData[MRT_DEPTH] = vec4((gl_FragCoord.z / gl_FragCoord.w - NearClipDistance) / (FarClipDistance - NearClipDistance), 0.0, 0.0, 0.0);
#else
    FragColor = vec4(LINEARtoSRGB(color.rgb), color.a);
#endif
}

#endif // OGRE_FRAGMENT_SHADER
#endif // MRT_HELPER_GLSL
