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

#define HAS_MRT
#include "header.glsl"

uniform sampler2D texMap;

uniform mediump vec4 FogColour;
uniform mediump vec4 FogParams;
uniform mediump float FarClipDistance;
uniform mediump float NearClipDistance;

in highp vec4 oUV;
in mediump vec4 oColour;
in mediump float oFogCoord;
void main()
{
    mediump vec4 s = texture2D(texMap, oUV.xy);
    mediump vec3 color = s.rgb;
    mediump float alpha = s.a;

#ifdef ALPHA_TEST
    if(alpha < 0.5 || oColour.a < 0.5)  {
        discard;
    }
#endif

    color = SRGBtoLINEAR(color);
    color = ApplyFog(color, FogParams, FogColour.rgb, oFogCoord);

#ifdef FORCE_SRGB
    color = LINEARtoSRGB(color);
#endif
#ifndef HAS_MRT
    FragColor = vec4(SafeHDR(color), alpha);
#else
    FragData[0] = vec4(SafeHDR(color), alpha);
    FragData[1] = vec4(0.0, 0.0, 1.0, 1.0);
    FragData[2] = vec4((oFogCoord - NearClipDistance) / (FarClipDistance - NearClipDistance), 0.0, 0.0, 1.0);
    FragData[3] = vec4(0.0, 0.0, 0.0, 1.0);
    FragData[4] = vec4(0.0, 0.0, 0.0, 1.0);
#endif
}
