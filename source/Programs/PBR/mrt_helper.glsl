// created by Andrey Vasiliev

#ifndef MRT_HELPER_GLSL
#define MRT_HELPER_GLSL
#ifdef OGRE_FRAGMENT_SHADER

#ifdef HAS_MRT
uniform highp float FarClipDistance;
uniform highp float NearClipDistance;
#endif

void EvaluateBuffer(const mediump vec3 color, const mediump float alpha)
{
    FragColor = vec4(color, alpha);
#ifdef HAS_MRT
    FragData[MRT_DEPTH] = vec4((gl_FragCoord.z / gl_FragCoord.w - NearClipDistance) / (FarClipDistance - NearClipDistance), 0.0, 0.0, 0.0);
#endif
}

void EvaluateBuffer(const mediump vec4 color)
{
    FragColor = vec4(color);
#ifdef HAS_MRT
    FragData[MRT_DEPTH] = vec4((gl_FragCoord.z / gl_FragCoord.w - NearClipDistance) / (FarClipDistance - NearClipDistance), 0.0, 0.0, 0.0);
#endif
}

#endif // OGRE_FRAGMENT_SHADER
#endif // MRT_HELPER_GLSL
