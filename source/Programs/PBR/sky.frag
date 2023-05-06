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


#define USE_MRT
#include "header.glsl"
#include "srgb.glsl"
#include "tonemap.glsl"


in highp vec3 vPosition;
in mediump vec3 vUV0;

uniform vec3 SunDirection;
uniform float SunSize;
uniform vec3 SunColor;
uniform highp vec3 A, B, C, D, E, F, G, H, I, Z;
uniform samplerCube CubeMap;


//----------------------------------------------------------------------------------------------------------------------
highp vec3 HosekWilkie(highp float cos_theta, highp float gamma, highp float cos_gamma)
{
    highp vec3 chi = ((1.0 + cos_gamma*cos_gamma) / pow(1.0 + I*I - 2.0*(cos_gamma*I), vec3(1.5, 1.5, 1.5)));
    return Z * ((1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E*gamma) + F * (cos_gamma*cos_gamma) + G * chi + H * sqrt(cos_theta)));
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    highp vec3 V = normalize(vPosition);
    highp vec3 N = normalize(-SunDirection);
    highp float cos_theta = clamp(V.y, 0.0, 1.0);
    highp float cos_gamma = dot(V, N);
    highp float gamma = acos(cos_gamma);
    mediump vec3 color = HosekWilkie(cos_theta, gamma, cos_gamma);

    color = XYZtoRGB(color);
    color = expose(color, 0.1);
    color = SRGBtoLINEAR(color);

    if (gamma <= SunSize) {
        color = SunColor;
    }

    FragData[0] = vec4(SafeHDR(color), 1.0);
    FragData[1] = vec4(0.0, 0.0, 1.0, 1.0);
    FragData[2] = vec4(0.0, 0.0, 0.0, 1.0);
    FragData[3] = vec4(1.0, 0.0, 0.0, 1.0);
    FragData[4] = vec4(0.0, 0.0, 0.0, 1.0);
    FragData[5] = vec4(0.0, 0.0, 0.0, 1.0);
}
