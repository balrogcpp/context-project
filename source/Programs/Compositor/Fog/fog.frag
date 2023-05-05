// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "fog.glsl"


in mediump vec2 vUV0;
uniform sampler2D RT;
uniform sampler2D DepthMap;
uniform mediump vec3 FogColour;
uniform mediump vec4 FogParams;
uniform mediump float FarClipDistance;
uniform mediump float NearClipDistance;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump float clampedDepth = texture2D(DepthMap, vUV0).x;

    if (clampedDepth > 0.5) {
        FragColor = vec4(color, 1.0);
        return;
    }
    
    mediump float depth = clampedDepth * FarClipDistance + NearClipDistance;
    mediump vec3 fog = ApplyFog(color, FogParams, FogColour, depth);
    FragColor = vec4(fog, 1.0);
}
