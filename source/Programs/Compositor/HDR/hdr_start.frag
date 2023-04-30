// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif

#include "header.glsl"
#include "tonemap.glsl"
#include "srgb.glsl"


in mediump vec2 vUV0;
uniform sampler2D RT;
uniform mediump vec2 TexSize0;
uniform mediump vec2 TexelSize0;
uniform mediump vec2 ViewportSize;

//const vec2 c_offsets[16] = vec2[16]
//(
//vec2( 0, 0 ), vec2( 1, 0 ), vec2( 0, 1 ), vec2( 1, 1 ),
//vec2( 2, 0 ), vec2( 3, 0 ), vec2( 2, 1 ), vec2( 3, 1 ),
//vec2( 0, 2 ), vec2( 1, 2 ), vec2( 0, 3 ), vec2( 1, 3 ),
//vec2( 2, 2 ), vec2( 3, 2 ), vec2( 2, 3 ), vec2( 3, 3 )
//);

//----------------------------------------------------------------------------------------------------------------------
//  https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L15
mediump float Downscale4x4(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = luminance(texture2D(tex, uv                         ).rgb) + 0.0001;
    mediump float B = luminance(texture2D(tex, uv + tsize * vec2(1.0, 0.0)).rgb) + 0.0001;
    mediump float C = luminance(texture2D(tex, uv + tsize * vec2(0.0, 1.0)).rgb) + 0.0001;
    mediump float D = luminance(texture2D(tex, uv + tsize * vec2(1.0, 1.0)).rgb) + 0.0001;
    mediump float E = luminance(texture2D(tex, uv + tsize * vec2(2.0, 0.0)).rgb) + 0.0001;
    mediump float F = luminance(texture2D(tex, uv + tsize * vec2(3.0, 0.0)).rgb) + 0.0001;
    mediump float G = luminance(texture2D(tex, uv + tsize * vec2(2.0, 1.0)).rgb) + 0.0001;
    mediump float H = luminance(texture2D(tex, uv + tsize * vec2(3.0, 1.0)).rgb) + 0.0001;
    mediump float I = luminance(texture2D(tex, uv + tsize * vec2(0.0, 2.0)).rgb) + 0.0001;
    mediump float J = luminance(texture2D(tex, uv + tsize * vec2(1.0, 2.0)).rgb) + 0.0001;
    mediump float K = luminance(texture2D(tex, uv + tsize * vec2(0.0, 3.0)).rgb) + 0.0001;
    mediump float L = luminance(texture2D(tex, uv + tsize * vec2(1.0, 3.0)).rgb) + 0.0001;
    mediump float M = luminance(texture2D(tex, uv + tsize * vec2(2.0, 2.0)).rgb) + 0.0001;
    mediump float N = luminance(texture2D(tex, uv + tsize * vec2(3.0, 2.0)).rgb) + 0.0001;
    mediump float O = luminance(texture2D(tex, uv + tsize * vec2(2.0, 3.0)).rgb) + 0.0001;
    mediump float P = luminance(texture2D(tex, uv + tsize * vec2(3.0, 4.0)).rgb) + 0.0001;

    mediump float c1 = (log(A * 1024.0) + log(B * 1024.0) + log(C * 1024.0) + log(D * 1024.0)) * 0.0625;
    mediump float c2 = (log(E * 1024.0) + log(F * 1024.0) + log(G * 1024.0) + log(H * 1024.0)) * 0.0625;
    mediump float c3 = (log(I * 1024.0) + log(J * 1024.0) + log(K * 1024.0) + log(L * 1024.0)) * 0.0625;
    mediump float c4 = (log(M * 1024.0) + log(N * 1024.0) + log(O * 1024.0) + log(P * 1024.0)) * 0.0625;

    return c1 + c2 + c3 + c4;
}


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    //Compute how many pixels we have to skip because we can't sample them all
    //e.g we have a 4096x4096 viewport (rt0), and we're rendering to a 64x64 surface
    //We would need 64x64 samples, but we only sample 4x4, therefore we sample one
    //pixel and skip 15, then repeat. We perform:
    //(ViewportResolution / TargetResolution) / 4
    mediump vec2 ratio = (TexelSize0 * ViewportSize) * 0.25;
    mediump float lum = Downscale4x4(RT, vUV0, TexelSize0 * ratio);

    FragColor = vec4(lum, lum, lum, 1.0);
}
