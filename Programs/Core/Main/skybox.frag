// This source file is part of "glue project". Created by Andrey Vasiliev

#ifndef GL_ES
#define VERSION 120
#version VERSION
#else
#define VERSION 100
#version VERSION
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

#ifndef GL_ES
    gl_FragData[0] = vec4(color, 1.0);
    gl_FragData[1] = vec4(1.0, 0.0, 0.0, 1.0);
#else
    color = ApplyFog(color, uFogParams, uFogColour, vDepth);
    gl_FragColor = vec4(LINEARtoSRGB(color, 1.0), 1.0);
#endif
}
