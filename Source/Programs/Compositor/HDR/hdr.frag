// created by Andrey Vasiliev
//? #version 400

#include "tonemap.glsl"

uniform mediump sampler2D RT;
uniform mediump sampler2D BrightTex;
uniform mediump sampler2D DirtTex;
uniform mediump sampler2D Lum;
uniform float Time;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
vec3 Upscale9(const sampler2D tex, const vec2 uv)
{
    vec3 A = textureLodOffset(tex, uv, 0.0, ivec2(-1, -1)).rgb;
    vec3 B = textureLodOffset(tex, uv, 0.0, ivec2( 0, -1)).rgb;
    vec3 C = textureLodOffset(tex, uv, 0.0, ivec2( 1, -1)).rgb;
    vec3 D = textureLodOffset(tex, uv, 0.0, ivec2(-1,  1)).rgb;
    vec3 E = textureLodOffset(tex, uv, 0.0, ivec2( 0,  0)).rgb;
    vec3 F = textureLodOffset(tex, uv, 0.0, ivec2( 1,  0)).rgb;
    vec3 G = textureLodOffset(tex, uv, 0.0, ivec2(-1,  1)).rgb;
    vec3 H = textureLodOffset(tex, uv, 0.0, ivec2( 0,  1)).rgb;
    vec3 I = textureLodOffset(tex, uv, 0.0, ivec2( 1,  1)).rgb;

    vec3 o = E * 0.25;
    o += (B + D + F + H) * 0.125;
    o += (A + C + G + I) * 0.0625;

    return o;
}

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L78
vec3 UpsampleBox(const sampler2D tex, const vec2 uv)
{
    vec3 o;
    o =  textureLodOffset(tex, uv, 0.0, ivec2(-1, -1)).rgb;
    o += textureLodOffset(tex, uv, 0.0, ivec2( 1, -1)).rgb;
    o += textureLodOffset(tex, uv, 0.0, ivec2(-1,  1)).rgb;
    o += textureLodOffset(tex, uv, 0.0, ivec2( 1,  1)).rgb;

    return o * 0.25;
}

// Applies a vignette effect
// color: source color to apply the vignette on top of
// uv: viewport coordinates
// vignette: pre-computed parameters midPoint, radius, aspect and feather
// vignetteColor: color of the vignette effect
vec3 vignette(const vec3 color, const highp vec2 uv, const vec4 vignette, const vec4 vignetteColor) {
    float midPoint = vignette.x;
    float radius = vignette.y;
    float aspect = vignette.z;
    float feather = vignette.w;

    vec2 distance = abs(uv - 0.5) * midPoint;
    distance.x *= aspect;
    distance = pow(clamp(distance, 0.0, 1.0), vec2(radius));

    float amount = pow(clamp(1.0 - dot(distance, distance), 0.0, 1.0), feather * 5.0);
    return color * mix(vignetteColor.rgb, vec3(1.0), amount);
}

vec3 grain(const vec3 color, const highp vec2 uv) {
    // Generate random noise
    float noise = (fract(sin(dot(uv * Time, vec2(12.9898, 78.233))) * 43758.5453) - 0.5) * 2.0;

    // Add noise to the original color
    return color + noise * 0.04;
}

out vec3 FragColor;
void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(textureSize(RT, 0));

    float lum = texelFetch(Lum, ivec2(0, 0), 0).r;
    vec3 bloom = UpsampleBox(BrightTex, uv).rgb;
    vec3 color = inverseTonemapSRGB(texelFetch(RT, ivec2(gl_FragCoord.xy), 0).rgb);
    vec3 dirt = textureLod(DirtTex, uv, 0.0).rgb * 10.0;
    color = mix(color, bloom + bloom * dirt, 0.04);
    color = tonemap(color * lum);
    color = grain(color, uv);
    //color = vignette(color, uv, vec4(1, 1, 1, 1), vec4(0.0, 0.0, 0.0, 1.0));
    FragColor.rgb = color;
}
