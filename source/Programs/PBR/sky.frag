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
#include "srgb.glsl"
#include "fog.glsl"
uniform mediump vec3 SunColor;
uniform highp vec3 SunDirection;
uniform highp float SunSize;
uniform mediump vec4 FogColour;
uniform mediump vec4 FogParams;
//uniform highp float FarClipDistance;
//uniform highp float NearClipDistance;

#ifdef GPU_HOSEK
#include "hosek.glsl"
#else
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
#endif

// Clamps color between 0 and 1 smoothly
highp vec3 SkyLightExpose(const highp vec3 color, const highp float exposure)
{
    return vec3(2.0, 2.0, 2.0) / (vec3(1.0, 1.0, 1.0) + exp(-exposure * color)) - vec3(1.0, 1.0, 1.0);
}

in highp vec3 vPosition;
in highp vec3 vUV0;
void main()
{
    highp vec3 V = normalize(vPosition);
    highp vec3 N = normalize(-SunDirection);
    highp float cos_theta = clamp(abs(V.y), 0.0, 1.0);
    highp float cos_gamma = dot(V, N);
    highp float gamma = acos(cos_gamma);
#ifdef GPU_HOSEK
    mediump vec3 color = sample_sky(cos_theta, gamma, N.y, N.x);
#else
    mediump vec3 color = HosekWilkie(cos_theta, gamma, cos_gamma);
#endif

    if (gamma <= SunSize && V.y > 0.0) {
        color = SunColor;
    }

    color = SkyLightExpose(color, 0.1);

#ifdef HAS_MRT
    color = SRGBtoLINEAR(color);
#endif
    EvaluateBuffer(SafeHDR(color), 1.0);
}
