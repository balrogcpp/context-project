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

//    mediump vec3 c1 = (D + E + I + J); c1 *= 0.125;
//    mediump vec3 c2 = (A + B + G + F); c2 *= 0.03125;
//    mediump vec3 c3 = (B + C + H + G); c3 *= 0.03125;
//    mediump vec3 c4 = (F + G + L + K); c4 *= 0.03125;
//    mediump vec3 c5 = (G + H + M + L); c5 *= 0.03125;

//    mediump vec3 c1 = (D + E + I + J); c1 /= (1.0 + luminance(LINEARtoSRGB(c1))); c1 *= 0.125;
//    mediump vec3 c2 = (A + B + G + F); c2 /= (1.0 + luminance(LINEARtoSRGB(c2))); c2 *= 0.03125;
//    mediump vec3 c3 = (B + C + H + G); c3 /= (1.0 + luminance(LINEARtoSRGB(c3))); c3 *= 0.03125;
//    mediump vec3 c4 = (F + G + L + K); c4 /= (1.0 + luminance(LINEARtoSRGB(c4))); c4 *= 0.03125;
//    mediump vec3 c5 = (G + H + M + L); c5 /= (1.0 + luminance(LINEARtoSRGB(c5))); c5 *= 0.03125;

    mediump vec3 c1 = (D + E + I + J); c1 /= (1.0 + luminance((c1))); c1 *= 0.125;
    mediump vec3 c2 = (A + B + G + F); c2 /= (1.0 + luminance((c2))); c2 *= 0.03125;
    mediump vec3 c3 = (B + C + H + G); c3 /= (1.0 + luminance((c3))); c3 *= 0.03125;
    mediump vec3 c4 = (F + G + L + K); c4 /= (1.0 + luminance((c4))); c4 *= 0.03125;
    mediump vec3 c5 = (G + H + M + L); c5 /= (1.0 + luminance((c5))); c5 *= 0.03125;


    return c1 + c2 + c3 + c4 + c5;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 Threshold(const mediump vec3 color, const mediump float threshold)
{
    return max(color - vec3(threshold), vec3(0.0, 0.0, 0.0));
}


in mediump vec2 vUV0;
uniform sampler2D ColorMap;
uniform mediump vec2 TexelSize0;
uniform mediump float Threshhold;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = Downscale13T(ColorMap, vUV0, TexelSize0);
    FragColor = vec4(Threshold(color, Threshhold), 1.0);
}
