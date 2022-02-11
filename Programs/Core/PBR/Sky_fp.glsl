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
//#include "hosek.glsl"
//#include "atmos.glsl"
#ifdef NO_MRT
#include "fog.glsl"
#endif

uniform vec3 uSunDirection;
//uniform float uTime;
#ifdef NO_MRT
uniform vec3 uFogColour;
uniform vec4 uFogParams;
#endif
//uniform samplerCube cubemap;
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

in vec3 vPosition;
in vec3 TexCoords;

vec3 HosekWilkie(float cos_theta, float gamma, float cos_gamma)
{
    vec3 chi = (1.0 + cos_gamma * cos_gamma) / pow(1.0 + H * H - 2.0 * cos_gamma * H, vec3(1.5));
    return (1.0 + A * exp(B / (cos_theta + 0.01))) * (C + D * exp(E * gamma) + F * (cos_gamma * cos_gamma) + G * chi + I * sqrt(cos_theta));
}

void main()
{
//    vec3 color = SRGBtoLINEAR(textureCube(cubemap, TexCoords).rgb);

    // Atmosphere Scattering
//    const float time = 0.0;
//    const float cirrus = 0.4;
//    const float cumulus = 0.8;
//    const float Br = 0.0025;
//    const float Bm = 0.0003;
//    const float g =  0.9800;
//    const vec3 nitrogen = vec3(0.650, 0.570, 0.475);
//    const vec3 Kr = Br / pow(nitrogen, vec3(4.0));
//    const vec3 Km = Bm / pow(nitrogen, vec3(0.84));
//    vec3 pos = vPosition;
//    vec3 fsun = -vec3(0, sin(0.1 * uTime), cos(0.1 * uTime));
//    float mu = dot(normalize(pos), normalize(fsun));
//    float rayleigh = 3.0 / (8.0 * 3.14) * (1.0 + mu * mu);
//    vec3 mie = (Kr + Km * (1.0 - g * g) / (2.0 + g * g) / pow(1.0 + g * g - 2.0 * g * mu, 1.5)) / (Br + Bm);
//    vec3 day_extinction = exp(-exp(-((pos.y + fsun.y * 4.0) * (exp(-pos.y * 16.0) + 0.1) / 80.0) / Br) * (exp(-pos.y * 16.0) + 0.1) * Kr / Br) * exp(-pos.y * exp(-pos.y * 8.0 ) * 4.0) * exp(-pos.y * 2.0) * 4.0;
//    vec3 night_extinction = vec3(1.0 - exp(fsun.y)) * 0.2;
//    vec3 extinction = mix(day_extinction, night_extinction, -fsun.y * 0.2 + 0.5);
//    vec3 color = rayleigh * mie * extinction;
//    color = SRGBtoLINEAR(color);

    vec3 V = normalize(vPosition);
    vec3 N = normalize(uSunDirection);
    float cos_theta = clamp(V.y, 0.0, 1.0);
    float cos_gamma = dot(V, N);
    float theta = acos(cos_theta);
    float gamma = acos(cos_gamma);
    vec3 color = Z * HosekWilkie(cos_theta, gamma, cos_gamma);
//    vec3 color = sample_sky(theta, gamma, N.y, N.x);
//    color = XYZ_to_RGB(color);
    color = expose(color, 0.1);
    color = SRGBtoLINEAR(color);

    // Cirrus Clouds
//    float density = smoothstep(1.0 - cirrus, 1.0, fbm(vPosition.xyz / vPosition.y * 2.0 + uTime * 0.05)) * 0.3;
//    color = mix(color, vec3(1.0), density * max(vPosition.y, 0.0));

    // Cumulus Clouds
//    for (int i = 0; i < 3; i++)
//    {
//        float density = smoothstep(1.0 - cumulus, 1.0, fbm((0.7 + float(i) * 0.01) * pos.xyz / pos.y + time * 0.3));
//        color.rgb = mix(color, vec3(1.0), min(density, 1.0) * max(pos.y, 0.0));
//    }

    // Dithering Noise
//    color.rgb += noise(vPosition * 1000) * 0.01;

#ifndef NO_MRT
    FragData[0].rgb = color;
//    FragData[1].r = 0.05;
#else
    color = ApplyFog(color, uFogParams, uFogColour, 0.05);
    FragColor.rgb = LINEARtoSRGB(color, 1.0);
#endif
}
