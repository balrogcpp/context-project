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
#include "mosaic.glsl"

uniform sampler2D MRT;
uniform sampler2D RT;
uniform sampler2D VelocityTex;
uniform sampler2D DepthTex;
uniform sampler2D DepthOldTex;
uniform mediump vec2 TexelSize0;
uniform mediump vec2 TexSize0;
uniform mediump mat4 InvViewMatrix;
uniform mediump mat4 ViewProjPrev;

// https://github.com/pezcode/mosaiikki/blob/e434b4dda52eba7c821591a5a4ac2de409e56244/src/Shaders/ReconstructionShader.frag#L242
mediump float ColorBlendWeight(const mediump vec3 a, const mediump vec3 b)
{
    return 1.0 / max(length(a - b), 0.001);
}

in highp vec2 vUV0;
in highp vec3 vRay;
void main()
{
    // pixel was rendered this frame = use it
    if (PixelWasRenderedThisFrame(vUV0, TexSize0)) {
        FragColor.rgb = SafeHDR(texture2D(MRT, vUV0).rgb);
        return;
    }

    // neighbors
    mediump vec3 A = texture2D(MRT, vUV0 + TexelSize0 * vec2(0.0, 1.0)).rgb;
    mediump vec3 B = texture2D(MRT, vUV0 + TexelSize0 * vec2(0.0, -1.0)).rgb;
    mediump vec3 C = texture2D(MRT, vUV0 + TexelSize0 * vec2(-1.0, 0.0)).rgb;
    mediump vec3 D = texture2D(MRT, vUV0 + TexelSize0 * vec2(1.0, 0.0)).rgb;

    // pixel was render prev frame, reconstruct it's position in prev frame
    highp vec2 velocity = texture2D(VelocityTex, vUV0).xy;
    highp float depth1 = texture2D(DepthTex, vUV0).x;

    if (Null(velocity)) {
        highp vec3 viewPos = vRay * depth1;
        highp vec4 worldPos = vec4(InvViewMatrix * vec4(viewPos, 1.0));
        worldPos.xyz /= worldPos.w;
        highp vec4 nuv = mul(ViewProjPrev, vec4(worldPos.xyz, 1.0));
        nuv.xy /= nuv.w;
        velocity = (nuv.xy - vUV0.xy);
    }

    highp float speed = length(velocity * TexSize0);
    velocity = TexelSize0 * floor(velocity * TexSize0);
    highp vec2 uv2 = vUV0 - velocity;
    highp float depth2 = texture2D(DepthOldTex, uv2).x;
    highp float diff = abs(depth2 - depth1);

    if (speed < 0.01 && diff < 0.01) {
        FragColor.rgb = SafeHDR(texture2D(RT, vUV0).rgb);
        return;
    }

    if (speed > 0.5 && PixelIsInsideViewport(uv2) && PixelWasRenderedPrevFrame(uv2, TexSize0) && diff < 0.01) {
        mediump vec3 color2 = texture2D(RT, uv2).rgb;
        mediump vec3 minColor = min(min(A, B), min(C, D));
        mediump vec3 maxColor = max(max(A, B), max(C, D));
        mediump vec3 clampedColor = clamp(color2, minColor, maxColor);

        // blend back towards reprojected result using confidence based on old depth
        // reuse depthTolerance to indicate 0 confidence cutoff
        // square falloff
        mediump float deviation = diff - 0.01;
        mediump float confidence = clamp(deviation * deviation, 0.0, 1.0);
        color2 = mix(clampedColor, color2, confidence);
        FragColor.rgb = SafeHDR(color2);
        return;
    }

    // https://github.com/pezcode/mosaiikki/blob/e434b4dda52eba7c821591a5a4ac2de409e56244/src/Shaders/ReconstructionShader.frag#L249
    mediump float verticalWeight = ColorBlendWeight(A, B);
    mediump float horizontalWeight = ColorBlendWeight(C, D);
    mediump vec3 result = (A + B) * verticalWeight + (C + D) * horizontalWeight;
    result = result * 0.5 * 1.0 / (verticalWeight + horizontalWeight);

    FragColor.rgb = SafeHDR(result);
}
