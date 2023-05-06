// created by Andrey Vasiliev

#if defined(OGRE_GLSL)
#version 150
#define __VERSION__ 150
#elif defined(OGRE_GLSLES)
#version 100
#define __VERSION__ 100
#endif


#include "header.glsl"


//----------------------------------------------------------------------------------------------------------------------
mediump float Gauss9HR(const sampler2D tex, const mediump vec2 uv, const mediump vec2 tsize)
{
    mediump float A = texture2D(tex, uv).r;
    mediump float B = texture2D(tex, uv + tsize * vec2(1.3846153846, 0.0)).r;
    mediump float C = texture2D(tex, uv - tsize * vec2(1.3846153846, 0.0)).r;
    mediump float D = texture2D(tex, uv + tsize * vec2(3.2307692308, 0.0)).r;
    mediump float E = texture2D(tex, uv - tsize * vec2(3.2307692308, 0.0)).r;

    mediump float color = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return color;
}


in mediump vec2 vUV0;
uniform sampler2D RT;
uniform mediump vec2 TexelSize0;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    float a = Gauss9HR(RT, vUV0, TexelSize0);
    FragColor = vec4(a, 0.0, 0.0, 1.0);
}
