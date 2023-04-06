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
float Gauss9VR(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    float A = texture2D(tex, uv).r;
    float B = texture2D(tex, uv + tsize * vec2(0.0, 1.3846153846)).r;
    float C = texture2D(tex, uv - tsize * vec2(0.0, 1.3846153846)).r;
    float D = texture2D(tex, uv + tsize * vec2(0.0, 3.2307692308)).r;
    float E = texture2D(tex, uv - tsize * vec2(0.0, 3.2307692308)).r;

    float color = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return color;
}


in vec2 vUV0;
uniform sampler2D RT;
uniform vec2 TexelSize0;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    FragColor.rgb = vec3(Gauss9VR(RT, vUV0, TexelSize0));
}
