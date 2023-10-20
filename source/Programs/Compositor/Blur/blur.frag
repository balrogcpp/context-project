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
uniform sampler2D DepthTex;
uniform sampler2D VelocityTex;
uniform mat4 InvViewMatrix;
uniform mat4 ViewProjPrev;
uniform float FrameTime;

in highp vec2 vUV0;
in highp vec3 vRay;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
    float clampedDepth = texture2D(DepthTex, vUV0).x;
    vec2 velocity = 0.5 * (FrameTime / 0.01666667) * texture2D(VelocityTex, vUV0).xy;

    if (Null(velocity)) {
        vec3 viewPos = vRay * clampedDepth;
        vec4 worldPos = vec4(InvViewMatrix * vec4(viewPos, 1.0));
        worldPos.xyz /= worldPos.w;
        vec4 nuv = mul(ViewProjPrev, vec4(worldPos.xyz, 1.0));
        nuv.xy /= nuv.w;
        velocity = 0.5 * (nuv.xy - vUV0.xy);
    }

    float speed = length(velocity * vec2(textureSize(VelocityTex, 0)));
    float nSamples = ceil(clamp(speed, 1.0, float(MAX_SAMPLES)));
    float invSamples = 1.0 / nSamples;

    for (int i = 1; i < MAX_SAMPLES; ++i) {
        if (int(nSamples) <= i) break;

        vec2 offset = (float(i) * invSamples - 0.5) * velocity;
        color += texture2D(RT, vUV0 + offset).rgb;
    }

    color *= invSamples;
    FragColor.rgb = SafeHDR(color);
}
