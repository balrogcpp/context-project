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

vec3 Gauss9H(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec3 A = texture2D(tex, uv).xyz;
    vec3 B = texture2D(tex, uv + tsize * vec2(1.3846153846, 0.0)).xyz;
    vec3 C = texture2D(tex, uv - tsize * vec2(1.3846153846, 0.0)).xyz;
    vec3 D = texture2D(tex, uv + tsize * vec2(3.2307692308, 0.0)).xyz;
    vec3 E = texture2D(tex, uv - tsize * vec2(3.2307692308, 0.0)).xyz;

    vec3 c1 = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return c1;
}

in vec2 vUV0;
void main()
{
    vec2 TexelSize0 = 1.0 / vec2(textureSize(RT, 0));
    FragColor.rgb = Gauss9H(RT, vUV0, TexelSize0);
}
