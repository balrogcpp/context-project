// created by Andrey Vasiliev
//? #version 400

#include "fx.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;
uniform sampler2D BrightTex;
uniform sampler2D DirtTex;
uniform sampler2D Lum;
uniform sampler2D LUT;


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
#if __VERSION__ > 330
    o.r = dot(textureGather(tex, uv, 0), vec4(1.0, 1.0, 1.0, 1.0));
    o.g = dot(textureGather(tex, uv, 1), vec4(1.0, 1.0, 1.0, 1.0));
    o.b = dot(textureGather(tex, uv, 2), vec4(1.0, 1.0, 1.0, 1.0));
#else
    ivec2 iuv = ivec2(gl_FragCoord.xy * 0.5);
    o =  texelFetchOffset(tex, iuv, 0, ivec2(-1, -1)).rgb;
    o += texelFetchOffset(tex, iuv, 0, ivec2( 1, -1)).rgb;
    o += texelFetchOffset(tex, iuv, 0, ivec2(-1,  1)).rgb;
    o += texelFetchOffset(tex, iuv, 0, ivec2( 1,  1)).rgb;
#endif 

    return o * 0.25;
}


out vec3 FragColor;
void main()
{
    vec2 uv = gl_FragCoord.xy / vec2(textureSize(RT, 0));
    vec3 color = inverseTonemapSRGB(texelFetch(RT, ivec2(gl_FragCoord.xy), 0).rgb);

    float lum = texelFetch(Lum, ivec2(0, 0), 0).r;
    vec3 bloom = UpsampleBox(BrightTex, uv);
    vec3 dirt = textureLod(DirtTex, uv, 0.0).rgb * 10.0;
    color = mix(color, bloom + bloom * dirt, 0.04);
    color *= lum;
    color = tonemap(color);

    color = applyPostEffects(color, uv, LUT);

    FragColor.rgb = color;
}
