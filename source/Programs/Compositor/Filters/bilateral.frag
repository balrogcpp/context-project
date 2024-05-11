// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform sampler2D DepthTex;
uniform vec2 TexelSize;

// https://github.com/nvpro-samples/gl_ssao/blob/f6b010dc7a05346518cd13d3368d8d830a382ed9/bilateralblur.frag.glsl
vec3 BlurFunction(const vec2 uv, float r, const vec3 center_c, float center_d, inout float w_total)
{
    #define KERNEL_RADIUS 3
    vec3 c = texture2D(RT, uv).rgb;
    float d = texture2D(DepthTex, uv).x;
    const float sharpness = 3.0;

    const float BlurSigma = float(KERNEL_RADIUS) * 0.5;
    const float BlurFalloff = 1.0 / (2.0*BlurSigma*BlurSigma);

    float ddiff = (d - center_d) * sharpness;
    float w = exp2(-r*r*BlurFalloff - ddiff*ddiff);
    w_total += w;

    return c*w;
}

in highp vec2 vUV0;
void main()
{
    vec3  center_c = texture2D(RT, vUV0).rgb;
    float center_d = texture2D(DepthTex, vUV0).x;

    vec3  c_total = center_c;
    float w_total = 1.0;

    for (int r = 1; r <= KERNEL_RADIUS; ++r)
    {
        c_total += BlurFunction(uv + TexSize4 * float(r), r, center_c, center_d, w_total);
    }
    for (int r = 1; r <= KERNEL_RADIUS; ++r)
    {
        c_total += BlurFunction(uv - TexSize4 * float(r), r, center_c, center_d, w_total);
    }

    FragColor.rgb = c_total/w_total;
}
