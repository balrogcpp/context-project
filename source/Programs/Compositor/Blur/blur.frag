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

#ifndef MAX_SAMPLES
#define MAX_SAMPLES 5
#endif

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D DepthSampler;
uniform mediump vec2 TexSize1;
uniform mediump mat4 InvViewMatrix;
uniform mediump mat4 ViewProjPrev;

in highp vec2 vUV0;
in highp vec3 vRay;
void main()
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    highp float clampedDepth = texture2D(DepthSampler, vUV0).x;
    highp vec3 viewPos = vRay * clampedDepth;
    highp vec4 worldPos = vec4(InvViewMatrix * vec4(viewPos, 1.0));
    worldPos.xyz /= worldPos.w;
    highp vec4 nuv = mul(ViewProjPrev, vec4(worldPos.xyz, 1.0));
    nuv.xy /= nuv.w;
    highp vec2 velocity = (nuv.xy - vUV0.xy);
    //mediump vec2 velocity = 0.5 * texture2D(VelocitySampler, vUV0).xy;

    mediump float speed = length(velocity * TexSize1);
    mediump float nSamples = ceil(clamp(speed, 1.0, float(MAX_SAMPLES)));
    mediump float invSamples = 1.0 / nSamples;

    for (int i = 1; i < MAX_SAMPLES; ++i) {
        if (int(nSamples) <= i) break;

        mediump vec2 offset = (float(i) * invSamples - 0.5) * velocity;
        color += texture2D(RT, vUV0 + offset).rgb;
    }

    color *= invSamples;

    FragColor = vec4(SafeHDR(color), 1.0);
}
