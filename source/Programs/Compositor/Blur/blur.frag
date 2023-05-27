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

SAMPLER2D(RT, 0);
SAMPLER2D(VelocitySampler, 1);

OGRE_UNIFORMS_BEGIN
uniform mediump vec2 TexSize1;
OGRE_UNIFORMS_END

MAIN_PARAMETERS
IN(mediump vec2 vUV0, TEXCOORD0)
MAIN_DECLARATION
{
    mediump vec3 color = texture2D(RT, vUV0).rgb;
    mediump vec2 velocity = texture2D(VelocitySampler, vUV0).xy;
    mediump float speed = length(velocity * TexSize1);
    mediump float nSamples = ceil(clamp(speed, 1.0, float(MAX_SAMPLES)));
    mediump float invSamples = 1.0 / nSamples;

    for (int i = 1; i < MAX_SAMPLES; ++i) {
        if (int(nSamples) <= i) break;

        vec2 offset = (float(i) * invSamples - 0.5) * velocity;
        color += texture2D(RT, vUV0 + offset).rgb;
    }

    color *= invSamples;

    FragColor = vec4(SafeHDR(color), 1.0);
}
