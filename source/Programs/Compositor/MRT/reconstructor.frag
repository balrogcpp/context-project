// created by Andrey Vasiliev

#include "header.glsl"
#include "mosaic.glsl"

uniform sampler2D MRT;
uniform sampler2D RT;
uniform sampler2D VelocityTex;
uniform sampler2D DepthTex;
uniform sampler2D DepthOldTex;
uniform vec4 ViewportSize;
uniform mat4 InvViewMatrix;
uniform mat4 ViewProjPrev;

// https://github.com/pezcode/mosaiikki/blob/e434b4dda52eba7c821591a5a4ac2de409e56244/src/Shaders/ReconstructionShader.frag#L242
float ColorBlendWeight(const vec3 a, const vec3 b)
{
    return 1.0 / max(length(a - b), 0.001);
}

vec2 RestoreTexCoord(const vec3 viewPos, const mat4 invViewMatrix, const mat4 viewProjPrev)
{
    vec4 worldPos = vec4(invViewMatrix * vec4(viewPos, 1.0));
    worldPos.xyz /= worldPos.w;
    vec4 nuv = mul(viewProjPrev, vec4(worldPos.xyz, 1.0));
    nuv.xy /= nuv.w;
    return nuv.xy;
}

in highp vec2 vUV0;
in highp vec3 vRay;
void main()
{
    // pixel was rendered this frame = use it
    if (PixelWasRenderedThisFrame()) {
        FragColor.rgb = SafeHDR(texture2D(MRT, vUV0).rgb);
        return;
    }

    // neighbors
    vec2 texSize0 = ViewportSize.xy;
    vec2 texelSize0 = ViewportSize.zw;
    vec3 A = texture2D(MRT, vUV0 + texelSize0 * vec2(0.0, 1.0)).rgb;
    vec3 B = texture2D(MRT, vUV0 + texelSize0 * vec2(0.0, -1.0)).rgb;
    vec3 C = texture2D(MRT, vUV0 + texelSize0 * vec2(-1.0, 0.0)).rgb;
    vec3 D = texture2D(MRT, vUV0 + texelSize0 * vec2(1.0, 0.0)).rgb;

    // pixel was render prev frame, reconstruct it's position in prev frame
    vec2 velocity = texture2D(VelocityTex, vUV0).xy;
    float depth1 = texture2D(DepthTex, vUV0).x;

    if (velocity == vec2(0.0, 0.0)) {
        vec3 viewPos = vRay * depth1;
        vec4 worldPos = vec4(InvViewMatrix * vec4(viewPos, 1.0));
        worldPos.xyz /= worldPos.w;
        vec4 nuv = mul(ViewProjPrev, vec4(worldPos.xyz, 1.0));
        nuv.xy /= nuv.w;
        velocity = (nuv.xy - vUV0.xy);
    }

    float speed = length(velocity * texSize0);
    velocity = texelSize0 * floor(velocity * texSize0);
    vec2 uv2 = vUV0 - velocity;
    float depth2 = texture2D(DepthOldTex, uv2).x;
    float diff = abs(depth2 - depth1);

    if (speed < 0.01 && diff < 0.01) {
        FragColor.rgb = SafeHDR(texture2D(RT, vUV0).rgb);
        return;
    }

    if (speed > 0.5 && PixelIsInsideViewport(uv2) && PixelWasRenderedPrevFrame() && diff < 0.01) {
        vec3 color2 = texture2D(RT, uv2).rgb;
        vec3 minColor = min(min(A, B), min(C, D));
        vec3 maxColor = max(max(A, B), max(C, D));
        vec3 clampedColor = clamp(color2, minColor, maxColor);

        // blend back towards reprojected result using confidence based on old depth
        // reuse depthTolerance to indicate 0 confidence cutoff
        // square falloff
        float deviation = diff - 0.01;
        float confidence = clamp(deviation * deviation, 0.0, 1.0);
        color2 = mix(clampedColor, color2, confidence);
        FragColor.rgb = SafeHDR(color2);
        return;
    }

    // https://github.com/pezcode/mosaiikki/blob/e434b4dda52eba7c821591a5a4ac2de409e56244/src/Shaders/ReconstructionShader.frag#L249
    float verticalWeight = ColorBlendWeight(A, B);
    float horizontalWeight = ColorBlendWeight(C, D);
    vec3 result = (A + B) * verticalWeight + (C + D) * horizontalWeight;
    result = result * 0.5 * 1.0 / (verticalWeight + horizontalWeight);

    FragColor.rgb = SafeHDR(result);
}
