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

SAMPLERCUBE(CubeMap, 0);

OGRE_UNIFORMS_BEGIN
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
OGRE_UNIFORMS_END

highp vec3 HosekWilkie(const highp float cos_theta, const highp float gamma, const highp float cos_gamma)
{
    highp vec3 chi = ((1.0 + cos_gamma*cos_gamma) / pow(1.0 + I*I - 2.0*(cos_gamma*I), vec3(1.5, 1.5, 1.5)));
    return Z * ((1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E*gamma) + F * (cos_gamma*cos_gamma) + G * chi + H * sqrt(cos_theta)));
}

MAIN_PARAMETERS
IN(highp vec3 vPosition, TEXCOORD0)
IN(highp vec3 vUV0, TEXCOORD1)
MAIN_DECLARATION
{
    highp vec3 V = normalize(vPosition);
    highp vec3 N = normalize(-SunDirection);
    highp float cos_theta = clamp(abs(V.y), 0.0, 1.0);
    highp float cos_gamma = dot(V, N);
    highp float gamma = acos(cos_gamma);
    mediump vec3 color = HosekWilkie(cos_theta, gamma, cos_gamma);

    color = XYZtoRGB(color);
    color = expose(color, 0.1);
    color = SRGBtoLINEAR(color);

    if (gamma <= SunSize && V.y > 0.0) {
        color = SunColor;
    }

#ifdef FORCE_SRGB
    color = LINEARtoSRGB(color);
#endif
#ifndef HAS_MRT
    FragColor = vec4(SafeHDR(color), 1.0);
#else
    FragData[0] = vec4(SafeHDR(color), 1.0);
    FragData[1] = vec4(0.0, 0.0, 1.0, 1.0);
    FragData[2] = vec4(1.0, 0.0, 0.0, 1.0);
    FragData[3] = vec4(0.0, 0.0, 0.0, 1.0);
    FragData[4] = vec4(0.0, 0.0, 0.0, 1.0);
#endif
}
