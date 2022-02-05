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
#include "fog.glsl"
#include "atmosphere.glsl"

uniform samplerCube cubemap;
uniform vec3 uFogColour;
uniform vec4 uFogParams;

#ifdef NO_MRT
in float vDepth;
#endif
in vec3 TexCoords; // direction vector representing a 3D texture coordinate

void main()
{
//    vec3 color = SRGBtoLINEAR(textureCube(cubemap, TexCoords).rgb);

    vec3 color = SRGBtoLINEAR(atmosphere(
                            normalize(TexCoords),           // normalized ray direction
                            vec3(0,6372e3,0),               // ray origin
                            vec3(0, 0.1, -1.0),       // position of the sun
                            22.0,                           // intensity of the sun
                            6371e3,                         // radius of the planet in meters
                            6471e3,                         // radius of the atmosphere in meters
                            vec3(5.5e-6, 13.0e-6, 22.4e-6), // Rayleigh scattering coefficient
                            21e-6,                          // Mie scattering coefficient
                            8e3,                            // Rayleigh scale height
                            1.2e3,                          // Mie scale height
                            0.758                           // Mie preferred scattering direction
                            ));

//    vec3 color = mix(color1, color2, 0.5);

#ifndef NO_MRT
    FragData[0].rgb = color;
    FragData[1].r = 0.2;
#else
    color = ApplyFog(color, uFogParams, uFogColour, vDepth);
    FragColor.rgb = LINEARtoSRGB(color, 1.0);
#endif
}
