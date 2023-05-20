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
#include "header.glsl"


SAMPLER2D(texMap, 0);
uniform mediump vec4 FogColour;
uniform mediump vec4 FogParams;
uniform mediump float FarClipDistance;
uniform mediump float NearClipDistance;

//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(highp vec4 oUV, TEXCOORD0)
IN(mediump vec4 oColour, TEXCOORD1)
IN(mediump float oFogCoord, TEXCOORD2)

MAIN_DECLARATION
{
    vec4 color = texture2D(texMap, oUV.xy);

#ifdef ALPHA_TEST
    if(color.a < 0.5 || oColour.a < 0.5)  {
        discard;
    }
#endif

    color.rgb = ApplyFog(color.rgb, FogParams, FogColour.rgb, oFogCoord);
    color.rgb = SRGBtoLINEAR(color.rgb);
    FragData[0] = color;
    FragData[1] = vec4(0.0, 0.0, 1.0, 1.0);
    FragData[2] = vec4((oFogCoord - NearClipDistance) / (FarClipDistance - NearClipDistance), 0.0, 0.0, 1.0);
    FragData[3] = vec4(0.0, 0.0, 0.0, 1.0);
    FragData[4] = vec4(0.0, 0.0, 0.0, 1.0);
}
