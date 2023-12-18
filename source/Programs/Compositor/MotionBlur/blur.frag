// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D DepthTex;
#ifdef MRT_VELOCITY
uniform sampler2D VelocityTex;
#endif
uniform mat4 InvViewMatrix;
uniform mat4 ViewProjPrev;
uniform float FPS;

#ifndef MAX_SAMPLES
#define MAX_SAMPLES 5
#endif

in highp vec2 vUV0;
in vec3 vRay;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
    float depth = texture2D(DepthTex, vUV0).x;
#ifdef MRT_VELOCITY
    vec2 velocity = texture2D(VelocityTex, vUV0).xy;
    if (Null(velocity)) {
#else
    vec2 velocity;
#endif
        vec3 viewPos = vRay * depth;
        vec4 worldPos = vec4(InvViewMatrix * vec4(viewPos, 1.0));
        worldPos.xyz /= worldPos.w;
        vec4 nuv = mul(ViewProjPrev, vec4(worldPos.xyz, 1.0));
        nuv.xy /= nuv.w;
        velocity = 0.5 * (nuv.xy - vUV0.xy);
#ifdef MRT_VELOCITY
    }
#endif
    velocity *= FPS / 60.0;
    float speed = length(velocity * vec2(textureSize(RT, 0)));
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
