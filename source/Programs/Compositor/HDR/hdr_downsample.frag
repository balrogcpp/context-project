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
uniform sampler2D RT;
uniform mediump vec2 TexelSize0;


//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/OGRECave/ogre-next/blob/v2.3.1/Samples/Media/2.0/scripts/materials/HDR/GLSL/DownScale02_SumLumIterative_ps.glsl
//----------------------------------------------------------------------------------------------------------------------
mediump float Downscale2x2(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).r;
    mediump float B = texture2D(tex, uv + (tsize * vec2(-1.0,  1.0))).r;
    mediump float C = texture2D(tex, uv + (tsize * vec2( 1.0, -1.0))).r;
    mediump float D = texture2D(tex, uv + (tsize * vec2( 1.0,  1.0))).r;

    mediump float c1 = (A + B + C + D) * 0.25;

    return c1;
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump float lum = Downscale2x2(RT, vUV0, TexelSize0);
    FragColor = vec4(lum, 0.0, 0.0, 1.0);
}
