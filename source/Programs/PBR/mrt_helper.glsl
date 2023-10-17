// created by Andrey Vasiliev

#ifndef MRT_HELPER_GLSL
#define MRT_HELPER_GLSL
#ifdef OGRE_FRAGMENT_SHADER

uniform highp float FarClipDistance;
uniform highp float NearClipDistance;

#ifndef HAS_MRT
#include "math.glsl"
#include "srgb.glsl"
#include "tonemap.glsl"
#endif

void EvaluateBuffer(mediump vec3 color, const mediump float alpha)
{
#ifdef HAS_MRT
    FragColor = vec4(SafeHDR(color), alpha);
    FragData[MRT_DEPTH] = vec4((gl_FragCoord.z / gl_FragCoord.w - NearClipDistance) / (FarClipDistance - NearClipDistance), 0.0, 0.0, 0.0);
#else
    FragColor = vec4(SafeHDR(unreal(color)), alpha);
#endif
}

void EvaluateBuffer(mediump vec3 color)
{
#ifdef HAS_MRT
    FragColor.rgb = SafeHDR(color);
    FragData[MRT_DEPTH] = vec4((gl_FragCoord.z / gl_FragCoord.w - NearClipDistance) / (FarClipDistance - NearClipDistance), 0.0, 0.0, 0.0);
#else
    FragColor.rgb = SafeHDR(unreal(color));
#endif
}

#endif // OGRE_FRAGMENT_SHADER
#endif // MRT_HELPER_GLSL
