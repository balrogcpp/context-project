// This source file is part of "glue project". Created by Andrey Vasiliev

#ifndef GL_ES
#version 330 core
#define VERSION 330
#else
#version 100
#define VERSION 100
#endif
#include "header.frag"

#include "srgb.glsl"
#include "fog.glsl"

uniform samplerCube cubemap;
uniform vec3 uFogColour;
uniform vec4 uFogParams;

in float vDepth;

in vec3 TexCoords; // direction vector representing a 3D texture coordinate


void main()
{
    vec3 color = SRGBtoLINEAR(textureCube(cubemap, TexCoords).rgb);

#ifndef NO_MRT
    FragData[0].rgb = color;
    FragData[1].r = 1.0;
#else
    color = ApplyFog(color, uFogParams, uFogColour, vDepth);
    FragColor = vec4(LINEARtoSRGB(color, 1.0), 1.0);
#endif
}
