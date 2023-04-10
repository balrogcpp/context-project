// created by Andrey Vasiliev

#ifndef GL_ES
#version 150
#define __VERSION__ 150
#else
#version 100
#define __VERSION__ 100
#endif


#include "header.frag"
#include "math.glsl"
#include "srgb.glsl"


#ifndef MAX_SAMPLES
#define MAX_SAMPLES 5
#endif


in vec2 vUV0;
uniform sampler2D ColorMap;
uniform sampler2D VelocitySampler;
uniform mediump vec2 TexSize1;
uniform mediump float Scale;


//----------------------------------------------------------------------------------------------------------------------
void main()
{
    mediump vec3 color = texture2D(ColorMap, vUV0).rgb;
    mediump vec2 velocity = Scale * texture2D(VelocitySampler, vUV0).xy;
    mediump float speed = length(velocity * TexSize1);
    mediump float nSamples = ceil(clamp(speed, 1.0, float(MAX_SAMPLES)));
    mediump float invSamples = 1.0 / nSamples;

    for (int i = 1; i < MAX_SAMPLES; ++i) {
        if (int(nSamples) <= i) break;

        vec2 offset = (float(i) * invSamples - 0.5) * velocity;
        color += texture2D(ColorMap, vUV0 + offset).rgb;
    }

    color *= invSamples;
    FragColor.rgb = color;
}
