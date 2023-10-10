// created by Andrey Vasiliev

#ifndef __VERSION__
#if defined(OGRE_GLSL)
#version 330 core
#define __VERSION__ 330
#elif defined(OGRE_GLSLES)
#version 300 es
#define __VERSION__ 300
#endif
#endif

#include "header.glsl"

// https://github.com/pezcode/mosaiikki/blob/e434b4dda52eba7c821591a5a4ac2de409e56244/src/Shaders/ReconstructionShader.frag#L242
mediump float colorBlendWeight(mediump vec3 a, mediump vec3 b)
{
    return 1.0 / max(length(a - b), 0.001);
}

uniform sampler2D MRT;
uniform sampler2D RT;
uniform sampler2D VelocityTex;
uniform sampler2D DepthTex;
uniform sampler2D DepthOldTex;
uniform mediump vec2 TexelSize0;
uniform mediump vec2 TexelSize1;
uniform mediump vec2 TexelSize2;
uniform mediump float IsEven;

in mediump vec2 vUV0;
void main()
{
    // pixel was rendered this frame = use it
    if (!(mod(floor(gl_FragCoord.y), 2.0) != IsEven && mod(floor(gl_FragCoord.x), 2.0) == IsEven)) {
        mediump vec3 color = texture2D(MRT, vUV0).rgb;
        if (Any(color)) {
            FragColor = vec4(SafeHDR(color), 1.0);
            return;
        }
    }

    // neighbors
    mediump vec3 A = texture2D(MRT, vUV0 + TexelSize0 * vec2(0.0, 1.0)).rgb;
    mediump vec3 B = texture2D(MRT, vUV0 + TexelSize0 * vec2(0.0, -1.0)).rgb;
    mediump vec3 C = texture2D(MRT, vUV0 + TexelSize0 * vec2(-1.0, 0.0)).rgb;
    mediump vec3 D = texture2D(MRT, vUV0 + TexelSize0 * vec2(1.0, 0.0)).rgb;

    // pixel was render prev frame, reconstruct it's position in prev frame
    mediump vec2 velocity = texture2D(VelocityTex, vUV0).xy;
    mediump vec3 color2 = texture2D(RT, vUV0 - TexelSize2 * velocity).rgb;
    mediump float depth1 = texture2D(DepthTex, vUV0).x;
    mediump float depth2 = texture2D(DepthOldTex, vUV0).x;
    if (Any(color2) && abs(depth2 - depth1) < 0.001) {
        FragColor = vec4(SafeHDR(color2), 1.0);
        return;
    }

    // https://github.com/pezcode/mosaiikki/blob/e434b4dda52eba7c821591a5a4ac2de409e56244/src/Shaders/ReconstructionShader.frag#L249
    mediump float verticalWeight = colorBlendWeight(A, B);
    mediump float horizontalWeight = colorBlendWeight(C, D);
    mediump vec3 result = (A + B) * verticalWeight + (C + D) * horizontalWeight;
    result = result * 0.5 * 1.0 / (verticalWeight + horizontalWeight);

    FragColor = vec4(SafeHDR(result), 1.0);
}
