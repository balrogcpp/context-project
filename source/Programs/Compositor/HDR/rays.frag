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
SAMPLER2D(FBO, 0);
uniform mediump float LightCount;
uniform mediump vec4 LightPositionViewSpace[MAX_LIGHTS];
uniform int RayCount;
uniform mediump float Decay;
uniform mediump float Density;
uniform mediump float Weight;
uniform mediump float Exposure;


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 GodRays(sampler2D tex, const mediump vec2 uv, const mediump vec2 lightPos, const int counter, const mediump float density, const mediump float weight, const mediump float decay, const mediump float exposure)
{
    mediump vec3 color = vec3(0.0, 0.0, 0.0);
    mediump vec2 suv = uv;
    mediump float illuminationDecay = 1.0;
    mediump vec2 deltaTextCoord = (uv - lightPos) * (density / float(counter));
    
    #define MAX_RAYS 100

    for(int i = 0; i < MAX_RAYS; ++i) {
        if (counter <= i) break;

        suv -= deltaTextCoord;
        color += texture2D(tex, suv).rgb * (illuminationDecay * weight);
        illuminationDecay *= decay;
    }

    color *= exposure;

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
MAIN_PARAMETERS
IN(mediump vec2 vUV0, TEXCOORD0);

MAIN_DECLARATION
{
    mediump vec3 color = vec3(0.0, 0.0, 0.0);

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (int(LightCount) <= i) break;

        mediump vec4 point = LightPositionViewSpace[i];
        float w = Weight * point.w;
        color += GodRays(FBO, vUV0, point.xy, RayCount, Density, w, Decay, Exposure);
    }

    FragColor = vec4(SafeHDR(color), 1.0);
}
