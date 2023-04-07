// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"


//----------------------------------------------------------------------------------------------------------------------
mediump float Gauss9VR(const mediump sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv).r;
    mediump float B = texture2D(tex, uv + tsize * vec2(0.0, 1.3846153846)).r;
    mediump float C = texture2D(tex, uv - tsize * vec2(0.0, 1.3846153846)).r;
    mediump float D = texture2D(tex, uv + tsize * vec2(0.0, 3.2307692308)).r;
    mediump float E = texture2D(tex, uv - tsize * vec2(0.0, 3.2307692308)).r;

    mediump float color = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return color;
}


in mediump vec2 vUV0;
uniform mediump sampler2D RT;
uniform mediump vec2 TexelSize0;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    FragColor.rgb = vec3(Gauss9VR(RT, vUV0, TexelSize0));
}
