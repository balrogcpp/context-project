// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D DepthTex;

// https://github.com/nvpro-samples/gl_ssao/blob/f6b010dc7a05346518cd13d3368d8d830a382ed9/bilateralblur.frag.glsl

#define KERNEL_RADIUS 3

vec3 BlurFunction(const vec2 uv, const float r, const vec3 center_c, const float center_d, inout const float w_total)
{
    vec3 c = textureLod(RT, uv, 0.0).rgb;
    float d = textureLod(DepthTex, uv, 0.0).x;
    const float sharpness = 3.0;

    const float BlurSigma = float(KERNEL_RADIUS) * 0.5;
    const float BlurFalloff = 1.0 / (2.0 * BlurSigma * BlurSigma);

    float ddiff = (d - center_d) * sharpness;
    float w = exp2(-(r * r * BlurFalloff) - ddiff * ddiff);
    w_total += w;

    return c*w;
}

out vec3 FragColor;
void main()
{
    vec2 size = vec2(textureSize(RT, 0));
    vec2 tsize = 1.0 / size;
    vec2 uv = gl_FragCoord.xy / size;
    uv.y = 1.0 - uv.y;

    vec3  center_c = textureLod(RT, uv, 0.0).rgb;
    float center_d = textureLod(DepthTex, uv, 0.0).x;

    vec3  c_total = center_c;
    float w_total = 1.0;

    for (int r = 1; r <= KERNEL_RADIUS; ++r)
    {
        c_total += BlurFunction(uv + tsize * float(r), float(r), center_c, center_d, w_total);
    }
    for (int r = 1; r <= KERNEL_RADIUS; ++r)
    {
        c_total += BlurFunction(uv - tsize * float(r), float(r), center_c, center_d, w_total);
    }

    FragColor.rgb = c_total / w_total;
}
