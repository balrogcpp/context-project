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

mediump float Gauss9H(sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv).x;
    mediump float B = texture2D(tex, uv + tsize * vec2(1.3846153846, 0.0)).x;
    mediump float C = texture2D(tex, uv - tsize * vec2(1.3846153846, 0.0)).x;
    mediump float D = texture2D(tex, uv + tsize * vec2(3.2307692308, 0.0)).x;
    mediump float E = texture2D(tex, uv - tsize * vec2(3.2307692308, 0.0)).x;

    mediump float c1 = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return c1;
}

uniform sampler2D RT;
uniform mediump vec2 TexelSize0;

in mediump vec2 vUV0;
void main()
{
    FragColor = vec4(Gauss9H(RT, vUV0, TexelSize0), 0.0, 0.0, 1.0);
}
