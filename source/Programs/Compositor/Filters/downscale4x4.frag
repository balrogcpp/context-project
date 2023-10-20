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

vec3 Downscale4x4(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec3 A = texture2D(tex, uv + tsize * vec2(-2.0, -2.0)).xyz;
    vec3 B = texture2D(tex, uv + tsize * vec2(-2.0, -1.0)).xyz;
    vec3 C = texture2D(tex, uv + tsize * vec2(-2.0,  0.0)).xyz;
    vec3 D = texture2D(tex, uv + tsize * vec2(-2.0,  1.0)).xyz;
    vec3 E = texture2D(tex, uv + tsize * vec2(-1.0, -2.0)).xyz;
    vec3 F = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).xyz;
    vec3 G = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).xyz;
    vec3 H = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).xyz;
    vec3 I = texture2D(tex, uv + tsize * vec2( 0.0, -2.0)).xyz;
    vec3 J = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).xyz;
    vec3 K = texture2D(tex, uv + tsize * vec2( 0.0,  0.0)).xyz;
    vec3 L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).xyz;
    vec3 M = texture2D(tex, uv + tsize * vec2( 1.0, -2.0)).xyz;
    vec3 N = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).xyz;
    vec3 O = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).xyz;
    vec3 P = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).xyz;

    vec3 c1 = (A + B + C + D) * 0.0625;
    vec3 c2 = (E + F + G + H) * 0.0625;
    vec3 c3 = (I + J + K + L) * 0.0625;
    vec3 c4 = (M + N + O + P) * 0.0625;

    return c1 + c2 + c3 + c4;
}

in vec2 vUV0;
void main()
{
    vec2 TexelSize0 = 1.0 / vec2(textureSize(RT, 0));
    FragColor.rgb = Downscale4x4(RT, vUV0, TexelSize0);
}
