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
uniform sampler2D OldLum;
uniform mediump vec2 TexelSize0;
uniform mediump vec3 Exposure;
uniform mediump float timeSinceLast;


//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L15
//----------------------------------------------------------------------------------------------------------------------
mediump float Downscale2x2(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv + (tsize * vec2(-1.0, -1.0))).r;
    mediump float B = texture2D(tex, uv + (tsize * vec2(-1.0,  1.0))).r;
    mediump float C = texture2D(tex, uv + (tsize * vec2( 1.0, -1.0))).r;
    mediump float D = texture2D(tex, uv + (tsize * vec2( 1.0,  1.0))).r;

    mediump float c1 = (A + B + C + D) * 0.25; // 1/4

    return c1;
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump float newLum = Downscale2x2(RT, vUV0, TexelSize0);
    newLum = expose2(newLum, Exposure);

    mediump float oldLum = texture2D(OldLum, vec2(0.0, 0.0)).r;

    mediump float lum = mix(newLum, oldLum, pow(0.25, timeSinceLast));

    FragColor = vec4(lum, lum, lum, 1.0);
}
