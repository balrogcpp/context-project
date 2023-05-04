// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif

#include "header.glsl"
#include "srgb.glsl"
#include "tonemap.glsl"


in mediump vec2 vUV0;
uniform sampler2D RT;
uniform mediump vec2 TexelSize0;


//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/OGRECave/ogre-next/blob/v2.3.1/Samples/Media/2.0/scripts/materials/HDR/GLSL/DownScale02_SumLumIterative_ps.glsl
//----------------------------------------------------------------------------------------------------------------------
mediump float Downscale2x2(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
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
    FragColor = vec4(lum, lum, lum, 1.0);
}
