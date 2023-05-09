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
uniform sampler2D OldLum;
uniform mediump vec2 TexelSize0;
uniform mediump vec3 Exposure;
uniform mediump float timeSinceLast;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump float newLum = Downscale2x2(RT, vUV0, TexelSize0);
    newLum = expose2(newLum, Exposure);

    mediump float oldLum = texture2D(OldLum, vec2(0.0, 0.0)).r;

    mediump float lum = mix(newLum, oldLum, pow(0.25, timeSinceLast));

    FragColor = vec4(lum, 0.0, 0.0, 1.0);
}
