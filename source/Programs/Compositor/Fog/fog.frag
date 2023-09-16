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

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D DepthMap;

uniform mediump vec4 FogColour;
uniform mediump vec4 FogParams;
uniform mediump float FarClipDistance;
uniform mediump float NearClipDistance;

in mediump vec2 vUV0;
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump float clampedDepth = texture2D(DepthMap, vUV0).x;

    if (clampedDepth > 0.5) {
        FragColor = vec4(color, 1.0);
        return;
    }
    
    mediump float depth = clampedDepth * (FarClipDistance - NearClipDistance) + NearClipDistance;
    mediump vec3 fog = ApplyFog(color, FogParams, FogColour.rgb, depth);
    FragColor = vec4(fog, 1.0);
}
