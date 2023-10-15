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
uniform sampler2D GlossTex;

in highp vec2 vUV0;
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump vec3 ssr = max(texture2D(SsrTex1, vUV0).rgb, texture2D(SsrTex2, vUV0).rgb);
    mediump vec2 uv = ssr.xy;
    mediump float fresnel = ssr.z;
    mediump float metallic = texture2D(GlossTex, vUV0).r;

    if (uv.x > HALF_EPSILON && uv.y > HALF_EPSILON && uv.x <= 1.0 && uv.y <= 1.0) {
        color = mix(color, texture2D(RT, uv).rgb, metallic * fresnel);
    }
    FragColor = vec4(SafeHDR(color), 1.0);
}
