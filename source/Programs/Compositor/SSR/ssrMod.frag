// created by Andrey Vasiliev

#ifndef __VERSION__
#if defined(OGRE_GLSL)
#version 330 core
#define __VERSION__ 330
#elif defined(OGRE_GLSLES)
#version 300 es
#define __VERSION__ 300
#endif
#endif

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D SsrTex1;
uniform sampler2D SsrTex2;

in highp vec2 vUV0;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
    vec3 ssr = max(texture2D(SsrTex1, vUV0).rgb, texture2D(SsrTex2, vUV0).rgb);
    vec2 uv = ssr.xy;
    float fresnel = ssr.z;

    if (Any(uv)) {
        color = mix(color, texture2D(RT, uv).rgb, fresnel);
    }
    FragColor.rgb = SafeHDR(color);
}
