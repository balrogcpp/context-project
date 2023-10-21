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

uniform sampler2D RT;
uniform sampler2D SsrTex;
uniform sampler2D GlossTex;

// https://github.com/pezcode/mosaiikki/blob/e434b4dda52eba7c821591a5a4ac2de409e56244/src/Shaders/ReconstructionShader.frag#L242
float ColorBlendWeight(const vec3 a, const vec3 b)
{
    return 1.0 / max(length(a - b), 0.001);
}

in vec2 vUV0;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
    vec3 ssr = texture2D(SsrTex, vUV0).rgb;
    vec2 uv = ssr.xy;
    float fresnel = ssr.z;
    float metallic = texture2D(SsrTex, vUV0).r;

    if (Any(uv)) {
        vec2 texSize0 = vec2(textureSize(RT, 0));
        vec2 texelSize0 = 1.0 / texSize0;
        vec3 A = texture2D(RT, uv + texelSize0 * vec2(0.0, 1.0)).rgb;
        vec3 B = texture2D(RT, uv + texelSize0 * vec2(0.0, -1.0)).rgb;
        vec3 C = texture2D(RT, uv + texelSize0 * vec2(-1.0, 0.0)).rgb;
        vec3 D = texture2D(RT, uv + texelSize0 * vec2(1.0, 0.0)).rgb;

        vec3 minColor = min(min(A, B), min(C, D));
        vec3 maxColor = max(max(A, B), max(C, D));
        vec3 clampedColor = clamp(0.25 * (A + B + C + D), minColor, maxColor);

        color = mix(color, clampedColor, fresnel * metallic);
    }

    FragColor.rgb = SafeHDR(color);
}
