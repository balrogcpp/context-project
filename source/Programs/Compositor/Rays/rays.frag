// created by Andrey Vasiliev

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "srgb.glsl"


varying mediump vec2 vUV0;
uniform sampler2D FboMap;
uniform mediump float LightCount;
uniform mediump vec4 LightPositionViewSpace[MAX_LIGHTS];
uniform mediump int RayCount;
uniform mediump float Decay;
uniform mediump float Density;
uniform mediump float Weight;
uniform mediump float Exposure;


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 GodRays(const sampler2D tex, const mediump vec2 uv, const mediump vec2 lightPos, const mediump int counter, const mediump float density, const mediump float weight, const mediump float decay, const mediump float exposure)
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

    return color;
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = vec3(0.0, 0.0, 0.0);

    for (int i = 0; i < MAX_LIGHTS; ++i) {
        if (int(LightCount) <= i) break;

        mediump vec4 point = LightPositionViewSpace[i];
        color += GodRays(FboMap, vUV0, point.xy, RayCount, Density, Weight * point.w, Decay, Exposure);
    }

    FragColor = vec4(color, 1.0);
}
