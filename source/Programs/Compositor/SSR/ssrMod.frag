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
#include "mosaic.glsl"

uniform sampler2D RT;
uniform sampler2D SsrTex;
uniform sampler2D GlossTex;

in vec2 vUV0;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
    vec3 ssr = texture2D(SsrTex, vUV0).rgb;
    vec2 uv = ssr.xy;
    float fresnel = ssr.z;
    float metallic = texture2D(SsrTex, vUV0).r;

    if (Any(uv)) {
        color = mix(color, texture2D(RT, uv).rgb, fresnel * metallic);
    }

    FragColor.rgb = SafeHDR(color);
}
