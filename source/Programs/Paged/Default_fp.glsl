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
#ifdef NO_MRT
#ifdef USE_FOG
#include "fog.glsl"
#endif
#endif

uniform sampler2D texMap;

#ifndef NO_MRT
uniform float uFarClipDistance;
#else
uniform vec4 fogColour;
uniform vec4 fogParams;
#endif

in vec4 oUV;
in vec4 oColour;
in float oFogCoord;

void main()
{
    vec4 color = texture2D(texMap, oUV.xy);
#ifdef ALPHA_TEST
    if(color.a < 0.5 || oColour.a < 0.5) discard;
#endif
    //color *= saturate(oColour);
    color = SRGBtoLINEAR(color);
#ifndef NO_MRT
    FragData[0] = color;
    FragData[1].r = float(oFogCoord / uFarClipDistance);
    //FragData[2].rg = vec2(0.0);
#else // NO_MRT
#ifdef USE_FOG
    color.rgb = ApplyFog(color.rgb, fogParams, fogColour.rgb, oFogCoord);
#endif // USE_FOG
    FragColor = LINEARtoSRGB(color, 1.0);
#endif // ! NO_MRT
}
