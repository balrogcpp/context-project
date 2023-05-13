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
uniform sampler2D AmbientMap;
uniform sampler2D EmissionMap;
uniform sampler2D SsaoMap;
uniform vec2 TexelSize3;
uniform vec4 ShadowColor;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump vec3 ambient = texture2D(AmbientMap, vUV0).rgb;
    mediump vec3 emission = texture2D(EmissionMap, vUV0).rgb;
    mediump float ssao = Upscale9(SsaoMap, vUV0, TexelSize3);

    color += clamp(ssao + ShadowColor.rgb, 0.0, 1.0) * ambient + emission;
    
    FragColor = vec4(SafeHDR(color), 1.0);
}
