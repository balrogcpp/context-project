// created by Andrey Vasiliev

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"
#include "filters_F16.glsl"


varying mediump vec2 vUV0;
uniform sampler2D RT;
uniform sampler2D SsaoMap;
uniform vec2 TexelSize1;
uniform vec4 ShadowColour;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump float ssao = Upscale9(SsaoMap, vUV0, TexelSize1);

    color *= clamp(ssao + ShadowColour.rgb, 0.0, 1.0);

    FragColor = vec4(SafeHDR(color), 1.0);
}
