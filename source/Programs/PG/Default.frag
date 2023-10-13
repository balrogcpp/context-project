// created by Andrey Vasiliev

#ifndef __VERSION__
#if defined(OGRE_GLSL)
#version 330 core
#define __VERSION__ 330
#elif defined(OGRE_GLSLES)
#version 300 es
#define __VERSION__ 300
#endif
#endif

#define HAS_MRT
#include "header.glsl"
#include "fog.glsl"
#include "srgb.glsl"

uniform sampler2D texMap;
uniform mediump vec4 FogColour;
uniform mediump vec4 FogParams;
uniform mediump float FarClipDistance;
uniform mediump float NearClipDistance;

in highp vec4 oUV;
in mediump vec4 oColour;
in mediump float oFogCoord;
in mediump vec4 vScreenPosition;
in mediump vec4 vPrevScreenPosition;
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

    FragData[MRT_COLOR] = vec4(SafeHDR(color), alpha);
    FragData[MRT_DEPTH] = vec4((oFogCoord - NearClipDistance) / (FarClipDistance - NearClipDistance), 0.0, 0.0, 0.0);
    FragData[MRT_VELOCITY] = vec4((vScreenPosition.xz / vScreenPosition.w) - (vPrevScreenPosition.xz / vPrevScreenPosition.w), 0.0, 0.0);
    FragData[MRT_NORMALS] = vec4(0.0, 0.0, 0.0, 0.0);
    FragData[MRT_GLOSS] = vec4(0.0, 0.0, 0.0, 0.0);
}
