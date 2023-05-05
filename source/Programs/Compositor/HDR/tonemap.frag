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


in mediump vec2 vUV0;
uniform sampler2D RT;


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 uncharted2Tonemap(mediump vec3 x) {
    const mediump float A = 0.22;
    const mediump float B = 0.3;
    const mediump float C = 0.10;
    const mediump float D = 0.20;
    const mediump float E = 0.01;
    const mediump float F = 0.30;
    const mediump float W = 11.2;

    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}


//----------------------------------------------------------------------------------------------------------------------
mediump vec3 uncharted2(const mediump vec3 color) {
    const mediump float W = 11.2;
    const mediump float exposureBias = 1.0;

    mediump vec3 curr = uncharted2Tonemap(exposureBias * color);
    return curr / uncharted2Tonemap(vec3(W, W, W));
}


//----------------------------------------------------------------------------------------------------------------------
mediump float uncharted2Tonemap(const mediump float x) {
    const mediump float A = 0.22;
    const mediump float B = 0.3;
    const mediump float C = 0.10;
    const mediump float D = 0.20;
    const mediump float E = 0.01;
    const mediump float F = 0.30;
    const mediump float W = 11.2;

    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}


//----------------------------------------------------------------------------------------------------------------------
mediump float uncharted2(const mediump float color) {
    const mediump float W = 11.2;
    const mediump float exposureBias = 1.0;

    mediump float curr = uncharted2Tonemap(exposureBias * color);

    return curr / uncharted2Tonemap(W);
}


// https://github.com/OGRECave/ogre-next/blob/v2.3.1/Samples/Media/2.0/scripts/materials/HDR/GLSL/FinalToneMapping_ps.glsl
//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    color = uncharted2(color);
    color  = (color - 0.5) * 1.25 + 0.5 + 0.11;

    FragColor = vec4(SafeHDR(color), 1.0);
}
