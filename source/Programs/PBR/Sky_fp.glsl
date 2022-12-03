// created by Andrey Vasiliev

#ifndef __VERSION__
#ifndef GL_ES
#version 330 core
#define __VERSION__ 330
#else
#version 300 es
#define __VERSION__ 300
#endif
#endif

#define USE_MRT
#include "header.frag"
#include "math.glsl"

in highp vec3 vPosition;
//in vec3 vUV0;

uniform vec3 uSunDirection;
#ifndef GPU_HOSEK
uniform vec3 A;
uniform vec3 B;
uniform vec3 C;
uniform vec3 D;
uniform vec3 E;
uniform vec3 F;
uniform vec3 G;
uniform vec3 H;
uniform vec3 I;
uniform vec3 Z;
#endif

#ifdef NO_MRT
uniform vec4 FogParams;
uniform float FarClipDistance;
#endif

uniform vec4 FogColour;
uniform float uSunSize;
uniform vec3 uSunColor;
//uniform float Time;
//uniform float uTimeScale;
//uniform float uCirrus;
//uniform float uCumulus;
//uniform samplerCube uCubeMap;


#ifdef GPU_HOSEK
#include "hosek.glsl"
#endif
#ifdef NO_MRT
#include "fog.glsl"
#endif
#include "atmos.glsl"
#include "srgb.glsl"


#ifndef GPU_HOSEK
//----------------------------------------------------------------------------------------------------------------------
vec3 HosekWilkie(float cos_theta, float gamma, float cos_gamma)
{
    vec3 chi = ((1.0 + cos_gamma*cos_gamma) / pow(1.0 + I*I - 2.0*(cos_gamma*I), vec3(1.5)));
    return Z * ((1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E*gamma) + F * (cos_gamma*cos_gamma) + G * chi + H * sqrt(cos_theta)));
}
#endif


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    highp vec3 V = normalize(vPosition);
    highp vec3 N = normalize(-uSunDirection);
    highp float cos_theta = clamp(V.y, 0.0, 1.0);
    highp float cos_gamma = dot(V, N);
    highp float gamma = acos(cos_gamma);
#ifndef GPU_HOSEK
    vec3 color = HosekWilkie(cos_theta, gamma, cos_gamma);
#else
    vec3 color = sample_sky(acos(cos_theta), gamma, N.y, N.x);
#endif

    color = XYZtoRGB(color);
    color = expose(color, 0.2);
    color = SRGBtoLINEAR(color);

    if (gamma <= uSunSize) color += uSunColor;
    //if (vPosition.y >= 0.0) color = ProceduralClouds(color, FogColour, vPosition, uCirrus, uCumulus, uTimeScale * Time);
    color = SafeHDR(color);

#ifndef NO_MRT
    FragData[0].rgb = color;
    FragData[1].r = 0.05;
#else
    color = ApplyFog(color, FogParams, FogColour.rgb, 0.05 * FarClipDistance);
    FragColor.rgb = LINEARtoSRGB(color, 1.0);
#endif
}
