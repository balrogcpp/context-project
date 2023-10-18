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

mediump vec3 Gauss9V(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv).xyz;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.3846153846)).xyz;
    mediump vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.3846153846)).xyz;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(0.0, 3.2307692308)).xyz;
    mediump vec3 E = texture2D(tex, uv - tsize * vec2(0.0, 3.2307692308)).xyz;

    mediump vec3 c1 = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return c1;
}

in highp vec2 vUV0;
void main()
{
    mediump vec2 TexelSize0 = 1.0 / vec2(textureSize(RT, 0));
    FragColor.rgb = Gauss9V(RT, vUV0, TexelSize0);
}
