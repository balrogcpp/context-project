// created by Andrey Vasiliev

#include "header.glsl"
#include "math.glsl"

uniform sampler2D RT;
uniform sampler2D DepthTex;
uniform vec4 ZBufferParams;
uniform mat4 InvViewMatrix;
uniform mat4 WorldViewProjMatrix;
uniform mat4 ViewProjPrev;
uniform float FrameTime;

#define MAX_SAMPLES 8

float Linear01Depth(const highp float z)
{
    return 1.0 / (z * ZBufferParams.x + ZBufferParams.y);
}


in highp vec3 vRay;
void main()
{
    vec2 uv = vec2(gl_FragCoord.xy) / vec2(textureSize(RT, 0));

    vec3 color = textureLod(RT, uv, 0.0).rgb;
    float depth = Linear01Depth(textureLod(DepthTex, vec2(uv.x, 1.0 - uv.y), 0.0).x);

    vec3 viewPos = vRay * depth;
    vec4 worldPos = mulMat4x4Half3(InvViewMatrix, viewPos.xyz);
    worldPos.xyz /= worldPos.w;

    vec4 nuv = mulMat4x4Half3(ViewProjPrev, worldPos.xyz);
    nuv.xy /= nuv.w;

    vec4 olduv = mulMat4x4Half3(WorldViewProjMatrix, worldPos.xyz);
    olduv.xy /= olduv.w;

    vec2 velocity = (nuv.xy - olduv.xy);

//    velocity *= 16.6666666667 /  FrameTime;
    float speed = length(velocity * vec2(textureSize(RT, 0)));
    float nSamples = ceil(clamp(speed, 1.0, float(MAX_SAMPLES)));
    float invSamples = 1.0 / nSamples;
    float counter = 1.0;

    for (int i = 1; i < MAX_SAMPLES; ++i) {
        if (int(nSamples) <= i) break;

        vec2 offset = (float(i) * invSamples - 0.5) * velocity;
        color += textureLod(RT, uv + offset, 0.0).rgb;
        counter += 1.0;
    }

    color /= counter;

    FragColor.rgb = color;
}
