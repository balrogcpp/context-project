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
#include "srgb.glsl"


uniform vec3 uSunDirection;
uniform float uSunSize;
uniform vec3 uSunColor;
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
uniform samplerCube uCubeMap;


in highp vec3 vPosition;
in vec3 vUV0;


//uniform float Time;
//uniform float uTimeScale;
//uniform float uCirrus;
//uniform float uCumulus;


//----------------------------------------------------------------------------------------------------------------------
vec3 HosekWilkie(float cos_theta, float gamma, float cos_gamma)
{
    vec3 chi = ((1.0 + cos_gamma*cos_gamma) / pow(1.0 + I*I - 2.0*(cos_gamma*I), vec3(1.5)));
    return Z * ((1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E*gamma) + F * (cos_gamma*cos_gamma) + G * chi + H * sqrt(cos_theta)));
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    highp vec3 V = normalize(vPosition);
    highp vec3 N = normalize(-uSunDirection);
    highp float cos_theta = clamp(V.y, 0.0, 1.0);
    highp float cos_gamma = dot(V, N);
    highp float gamma = acos(cos_gamma);
    vec3 color = HosekWilkie(cos_theta, gamma, cos_gamma);

    color = XYZtoRGB(color);
    color = expose(color, 0.1);
    color = SRGBtoLINEAR(color);

    if (gamma <= uSunSize) color += uSunColor;
    //color += SRGBtoLINEAR(textureCube(uCubeMap, vUV0).rgb);
    //if (vPosition.y >= 0.0) color = ProceduralClouds(color, FogColour, vPosition, uCirrus, uCumulus, uTimeScale * Time);

    FragData[0].rgb = color;
    FragData[1] = vec4(0.0, 0.0, 0.0, 1.0);
}
