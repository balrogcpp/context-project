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

#define HAS_MRT
#include "header.glsl"
#include "hosek.glsl"

uniform mediump vec3 SunColor;
uniform highp vec3 SunDirection;
uniform highp float SunSize;
uniform highp vec3 A;
uniform highp vec3 B;
uniform highp vec3 C;
uniform highp vec3 D;
uniform highp vec3 E;
uniform highp vec3 F;
uniform highp vec3 G;
uniform highp vec3 H;
uniform highp vec3 I;
uniform highp vec3 Z;

highp vec3 HosekWilkie(const highp float cos_theta, const highp float gamma, const highp float cos_gamma)
{
    highp float cos_gamma2 = cos_gamma * cos_gamma;
    highp vec3 I2 = I * I;
    highp vec3 chi = ((1.0 + cos_gamma2) / pow(1.0 + I2 - 2.0 * (cos_gamma * I), vec3(1.5, 1.5, 1.5)));
    return Z * ((1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * cos_gamma2 + G * chi + H * sqrt(cos_theta)));
}

in highp vec3 vPosition;
in highp vec3 vUV0;
void main()
{
#ifdef HAS_MRT
    FragData[MRT_DEPTH] = vec4(1.0, 0.0, 0.0, 1.0);
    FragData[MRT_VELOCITY] = vec4(0.0, 0.0, 0.0, 1.0);
    FragData[MRT_NORMALS] = vec4(0.0, 0.0, 0.0, 1.0);
    FragData[MRT_GLOSS] = vec4(0.0, 0.0, 1.0, 1.0);
#endif

#ifdef CHECKERBOARD
    if (ExcludePixel()) return;
#endif

    highp vec3 V = normalize(vPosition);
    highp vec3 N = normalize(-SunDirection);
    highp float cos_theta = clamp(abs(V.y), 0.0, 1.0);
    highp float cos_gamma = dot(V, N);
    highp float gamma = acos(cos_gamma);
//    mediump vec3 color = HosekWilkie(cos_theta, gamma, cos_gamma);
    vec3 color = sample_sky(cos_theta, gamma, N.y, N.x);

    color = XYZtoRGB(color);
    color = SkyLightExpose(color, 0.1);
    color = SRGBtoLINEAR(color);

    if (gamma <= SunSize && V.y > 0.0) {
        color = SunColor;
    }

#ifdef FORCE_SRGB
    color = LINEARtoSRGB(color);
#endif
    FragData[MRT_COLOR] = vec4(SafeHDR(color), 1.0);
}
