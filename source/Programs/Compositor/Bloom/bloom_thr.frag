// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "srgb.glsl"
#include "tonemap.glsl"


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Downscale13T(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump vec3 A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).rgb;
    mediump vec3 B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).rgb;
    mediump vec3 C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).rgb;
    mediump vec3 D = texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).rgb;
    mediump vec3 E = texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).rgb;
    mediump vec3 F = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).rgb;
    mediump vec3 G = texture2D(tex, uv                           ).rgb;
    mediump vec3 H = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).rgb;
    mediump vec3 I = texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).rgb;
    mediump vec3 J = texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).rgb;
    mediump vec3 K = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).rgb;
    mediump vec3 L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).rgb;
    mediump vec3 M = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).rgb;

    mediump vec3 c1 = (D + E + I + J) * 0.125;
    mediump vec3 c2 = (A + B + G + F) * 0.03125;
    mediump vec3 c3 = (B + C + H + G) * 0.03125;
    mediump vec3 c4 = (F + G + L + K) * 0.03125;
    mediump vec3 c5 = (G + H + M + L) * 0.03125;

    return c1 + c2 + c3 + c4 + c5;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Pass(const mediump vec3 color, const mediump vec2 threshold)
{
    mediump vec3 w = clamp((color - threshold.x) * threshold.y, 0.0, 1.0);
    return (color * (w * (w * (3.0 - 2.0 * w)))) * 0.0625;
}


in mediump vec2 vUV0;
uniform sampler2D ColorMap;
uniform sampler2D Lum;
uniform mediump vec2 TexelSize0;
uniform mediump vec2 Threshold;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = Downscale13T(ColorMap, vUV0, TexelSize0);
    mediump float lum = texture2D(Lum, vec2(0.0, 0.0)).r;

    color *= lum;

    FragColor = vec4(Pass(color, Threshold), 1.0);
}
