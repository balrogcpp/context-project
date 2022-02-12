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

#include "srgb.glsl"
#include "hosek.glsl"
#include "atmos.glsl"
#ifdef NO_MRT
#include "fog.glsl"
#endif

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
#endif
uniform float uTime;
uniform samplerCube cubemap;

in vec3 vPosition;
in vec3 TexCoords;

vec3 HosekWilkie(float cos_theta, float gamma, float cos_gamma)
{
    vec3 chi = (1.0 + cos_gamma * cos_gamma) / pow(1.0 + H * H - 2.0 * cos_gamma * H, vec3(1.5));
    return (1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * (cos_gamma * cos_gamma) + G * chi + I * sqrt(cos_theta));
}

void main()
{
    vec3 V = normalize(vPosition);
    vec3 N = normalize(uSunDirection);
    float cos_theta = clamp(V.y, 0.0, 1.0);
    float cos_gamma = dot(V, N);
    float theta = acos(cos_theta);
    float gamma = acos(cos_gamma);
    vec3 color = Z * HosekWilkie(cos_theta, gamma, cos_gamma);

#ifndef NO_MRT
    FragData[0].rgb = color;
    FragData[1].r = 0.05;
#else
    color = ApplyFog(color, uFogParams, uFogColour, 0.05);
    FragColor.rgb = LINEARtoSRGB(color, 1.0);
#endif
}
