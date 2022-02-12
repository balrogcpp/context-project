// This source file is part of "glue project". Created by Andrey Vasiliev

#ifndef VERSION
#ifndef GL_ES
#version 330 core
#define VERSION 330
#else
#version 100
#define VERSION 100
#endif
#endif

#include "header.frag"

uniform vec3 uSunDirection;
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

#ifdef NO_MRT
uniform vec3 uFogColour;
uniform vec4 uFogParams;
uniform float cFarClipDistance;
#endif
//uniform float uTime;
uniform samplerCube uCubeMap;

in vec3 vPosition;
in vec3 TexCoords;

#include "srgb.glsl"
#ifdef NO_MRT
#include "fog.glsl"
#endif

vec3 HosekWilkie(float cos_theta, float gamma, float cos_gamma)
{
    vec3 chi = (1.0 + cos_gamma*cos_gamma) / pow(1.0 + I*I - 2.0*cos_gamma*I, vec3(1.5));
    return (1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E*gamma) + F * (cos_gamma*cos_gamma) + G * chi + H * sqrt(cos_theta));
}

void main()
{
    vec3 V = normalize(vPosition);
    vec3 N = normalize(-uSunDirection);
    float cos_theta = clamp(V.y, 0.0, 1.0);
    float cos_gamma = dot(V, N);
    float gamma = acos(cos_gamma);
    vec3 color = Z * HosekWilkie(cos_theta, gamma, cos_gamma);
    if (cos_gamma > 0.0) color += pow(vec3(cos_gamma), vec3(256)) * 0.5;
    if (cos_gamma >= 0.999) color += vec3(20000.0);
    //color = mix(color, SRGBtoLINEAR(textureCube(uCubeMap, TexCoords).rgb), 0.5);

#ifndef NO_MRT
    FragData[0].rgb = color;
    FragData[1].r = 0.05;
#else
    color = ApplyFog(color, uFogParams, uFogColour, 0.05 * cFarClipDistance);
    FragColor.rgb = LINEARtoSRGB(color, 1.0);
#endif
}
