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
uniform sampler2D SsaoTex;
uniform vec4 ShadowColour;

float Gauss9V(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    float A = texture2D(tex, uv).x;
    float B = texture2D(tex, uv + tsize * vec2(0.0, 1.3846153846)).x;
    float C = texture2D(tex, uv - tsize * vec2(0.0, 1.3846153846)).x;
    float D = texture2D(tex, uv + tsize * vec2(0.0, 3.2307692308)).x;
    float E = texture2D(tex, uv - tsize * vec2(0.0, 3.2307692308)).x;

    float c1 = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return c1;
}

in vec2 vUV0;
void main()
{
    vec2 TexelSize0 = 1.0 / vec2(textureSize(RT, 0));
    vec3 color = texture2D(RT, vUV0).rgb;
    float ssao = Gauss9V(SsaoTex, vUV0, TexelSize0);

    color *= clamp(ssao + ShadowColour.g, 0.0, 1.0);
    FragColor.rgb = SafeHDR(color);
}
