// created by Andrey Vasiliev

#include "header.glsl"
#include "tonemap.glsl"

uniform sampler2D RT;
uniform sampler2D BrightTex;
uniform sampler2D DirtTex;
uniform sampler2D Lum;

// https://github.com/Unity-Technologies/Graphics/blob/f86c03aa3b20de845d1cf1a31ee18aaf14f94b41/com.unity.postprocessing/PostProcessing/Shaders/Sampling.hlsl#L57
vec3 Upscale9(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec4 d = tsize.xyxy * vec4(1.0, 1.0, -1.0, 0.0);

    vec3 A = textureLod(tex, uv - d.xy, 0.0).rgb;
    vec3 B = textureLod(tex, uv - d.wy, 0.0).rgb;
    vec3 C = textureLod(tex, uv - d.zy, 0.0).rgb;
    vec3 D = textureLod(tex, uv + d.zw, 0.0).rgb;
    vec3 E = textureLod(tex, uv       , 0.0).rgb;
    vec3 F = textureLod(tex, uv + d.xw, 0.0).rgb;
    vec3 G = textureLod(tex, uv + d.zy, 0.0).rgb;
    vec3 H = textureLod(tex, uv + d.wy, 0.0).rgb;
    vec3 I = textureLod(tex, uv + d.xy, 0.0).rgb;

    vec3 c = E * 0.25;
    c += (B + D + F + H) * 0.125;
    c += (A + C + G + I) * 0.0625;

    return c;
}

// Standard box filtering
vec3 UpsampleBox(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec4 d = tsize.xyxy * vec4(-1.0, -1.0, 1.0, 1.0) * 0.5;

    vec3 s;
    s =  textureLod(tex, uv + d.xy, 0.0).rgb;
    s += textureLod(tex, uv + d.zy, 0.0).rgb;
    s += textureLod(tex, uv + d.xw, 0.0).rgb;
    s += textureLod(tex, uv + d.zw, 0.0).rgb;

    return s * (1.0 / 4.0);
}

// Unreal 3, Documentation: "Color Grading"
// Adapted to be close to Tonemap_ACES, with similar range
// Gamma 2.2 correction is baked in, don't use with sRGB conversion!
// vec3 unreal(const highp vec3 x)
// {
//     return x / (x + 0.155) * 1.019;
// }

void main()
{
    vec2 size = vec2(textureSize(RT, 0));
    vec2 tsize = 1.0 / size;
    vec2 uv = gl_FragCoord.xy / size;
    uv.y = 1.0 - uv.y;

    float lum = texelFetch(Lum, ivec2(0, 0), 0).r;
    vec3 bloom = UpsampleBox(BrightTex, uv, tsize).rgb;
    vec3 color = inverseTonemapSRGB(texelFetch(RT, ivec2(gl_FragCoord.xy), 0).rgb);
    vec3 dirt = textureLod(DirtTex, uv, 0.0).rgb * 10.0;
    color = mix(color, bloom + bloom * dirt, 0.04);
    FragColor.rgb = unreal(color * lum);
}
