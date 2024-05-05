// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D DepthTex;
uniform sampler2D VelocityTex;
uniform vec2 TexSize;
uniform mat4 InvViewMatrix;
uniform mat4 WorldViewProjMatrix;
uniform mat4 ViewProjPrev;
uniform vec4 ViewportSize;
uniform float FrameTime;

#ifndef MAX_SAMPLES
#ifndef GL_ES
#define MAX_SAMPLES 8
#else
#define MAX_SAMPLES 8
#endif
#endif

in highp vec2 vUV0;
in highp vec3 vRay;
void main()
{
    vec3 color = texture2D(RT, vUV0).rgb;
    float depth = texture2D(DepthTex, vUV0).x;
    vec2 velocity = texture2D(VelocityTex, vUV0).xy;

    if (velocity == vec2(0.0, 0.0)) {
        vec3 viewPos = vRay * depth;
        vec4 worldPos = mulMat4x4Float3(InvViewMatrix, viewPos.xyz);
        worldPos.xyz /= worldPos.w;

        vec4 nuv = mulMat4x4Float3(ViewProjPrev, worldPos.xyz);
        nuv.xy /= nuv.w;

        vec4 olduv = mulMat4x4Float3(WorldViewProjMatrix, worldPos.xyz);
        olduv.xy /= olduv.w;

        velocity = (nuv.xy - olduv.xy);
    }

    velocity *= 16.6666666667 /  FrameTime;
    float speed = length(velocity * ViewportSize.xy);
    float nSamples = ceil(clamp(speed, 1.0, float(MAX_SAMPLES)));
    float invSamples = 1.0 / nSamples;
    float counter = 1.0;

    for (int i = 1; i < MAX_SAMPLES; ++i) {
        if (int(nSamples) <= i) break;

        vec2 offset = (float(i) * invSamples - 0.5) * velocity;
        float dDepth = (depth - texture2D(DepthTex, vUV0 + offset).r);
        if (dDepth > 0.001) break;
        color += texture2D(RT, vUV0 + offset).rgb;
        counter += 1.0;
    }

    color /= counter;
    FragColor.rgb = SafeHDR(color);
}
