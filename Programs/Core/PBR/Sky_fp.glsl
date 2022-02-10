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
uniform float uTime;
uniform vec3 uFogColour;
uniform vec4 uFogParams;
uniform samplerCube cubemap;

in vec3 vPosition;
in vec3 TexCoords;

void main()
{
//    vec3 color = SRGBtoLINEAR(textureCube(cubemap, TexCoords).rgb);

    // Atmosphere Scattering
//    vec3 pos = vPosition;
//    vec3 fsun = vec3(0, sin(0.1 * uTime), cos(0.1 * uTime));
//    float mu = dot(normalize(pos), normalize(fsun));
//    float rayleigh = 3.0 / (8.0 * 3.14) * (1.0 + mu * mu);
//    vec3 mie = (Kr + Km * (1.0 - g * g) / (2.0 + g * g) / pow(1.0 + g * g - 2.0 * g * mu, 1.5)) / (Br + Bm);
//    vec3 day_extinction = exp(-exp(-((pos.y + fsun.y * 4.0) * (exp(-pos.y * 16.0) + 0.1) / 80.0) / Br) * (exp(-pos.y * 16.0) + 0.1) * Kr / Br) * exp(-pos.y * exp(-pos.y * 8.0 ) * 4.0) * exp(-pos.y * 2.0) * 4.0;
//    vec3 night_extinction = vec3(1.0 - exp(fsun.y)) * 0.2;
//    vec3 extinction = mix(day_extinction, night_extinction, -fsun.y * 0.2 + 0.5);
//    vec3 color = rayleigh * mie * extinction;
//    color = SRGBtoLINEAR(color);

    vec3 V = normalize(vPosition);
    float cos_theta = clamp(V.y, 0.0, 1.0);
    float cos_gamma = dot(V, -uSunDirection);
    float theta = acos(cos_theta);
    float gamma = acos(cos_gamma);
    vec3 N = -normalize(uSunDirection);
    vec3 color = sample_sky(theta, gamma, N.y, N.x);
    color = XYZ_to_RGB(color);
    color = expose(color, 0.1);
    color = SRGBtoLINEAR(color);

    // Cirrus Clouds
//    float density = smoothstep(1.0 - cirrus, 1.0, fbm(pos.xyz / pos.y * 2.0 + uTime * 0.05)) * 0.3;
//    color = mix(color, vec3(1.0), density * max(pos.y, 0.0));

#ifndef NO_MRT
    FragData[0].rgb = color;
    FragData[1].r = 0.05;
#else
    color = ApplyFog(color, uFogParams, uFogColour, 0.05);
    FragColor.rgb = LINEARtoSRGB(color, 1.0);
#endif
}
