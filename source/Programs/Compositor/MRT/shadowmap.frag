// created by Andrey Vasiliev

#include "header.glsl"

uniform mediump sampler2D ShadowMap0;
uniform mediump sampler2D ShadowMap1;
uniform mediump sampler2D ShadowMap2;
uniform mediump sampler2D ShadowMap3;
//uniform mediump sampler2D ShadowMap4;
//uniform mediump sampler2D ShadowMap5;
//uniform mediump sampler2D ShadowMap6;
//uniform mediump sampler2D ShadowMap7;

float Downscale13_Log(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    const float W = 13.0;

    float G = texture2D(tex, uv).x;
    float A = exp(W * (texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).x - G));
    float B = exp(W * (texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).x - G));
    float C = exp(W * (texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).x - G));
    float D = exp(W * (texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).x - G));
    float E = exp(W * (texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).x - G));
    float F = exp(W * (texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).x - G));
    float H = exp(W * (texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).x - G));
    float I = exp(W * (texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).x - G));
    float J = exp(W * (texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).x - G));
    float K = exp(W * (texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).x - G));
    float L = exp(W * (texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).x - G));
    float M = exp(W * (texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).x - G));

    float c1 = (D + E + I + J) * 0.125;
    float c2 = (A + B + G + F) * 0.03125;
    float c3 = (B + C + H + 1.0) * 0.03125;
    float c4 = (F + G + L + K) * 0.03125;
    float c5 = (G + H + M + L) * 0.03125;

    return G + log(c1 + c2 + c3 + c4 + c5) / W;
}

float Downscale13(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    float A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).x;
    float B = texture2D(tex, uv + tsize * vec2( 0.0, -1.0)).x;
    float C = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).x;
    float D = texture2D(tex, uv + tsize * vec2(-0.5, -0.5)).x;
    float E = texture2D(tex, uv + tsize * vec2( 0.5, -0.5)).x;
    float F = texture2D(tex, uv + tsize * vec2(-1.0,  0.0)).x;
    float G = texture2D(tex, uv                           ).x;
    float H = texture2D(tex, uv + tsize * vec2( 1.0,  0.0)).x;
    float I = texture2D(tex, uv + tsize * vec2(-0.5,  0.5)).x;
    float J = texture2D(tex, uv + tsize * vec2( 0.5,  0.5)).x;
    float K = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).x;
    float L = texture2D(tex, uv + tsize * vec2( 0.0,  1.0)).x;
    float M = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).x;

    float c1 = (D + E + I + J) * 0.125;
    float c2 = (A + B + G + F) * 0.03125;
    float c3 = (B + C + H + G) * 0.03125;
    float c4 = (F + G + L + K) * 0.03125;
    float c5 = (G + H + M + L) * 0.03125;

    return c1 + c2 + c3 + c4 + c5;
}

float Box4(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    float A = texture2D(tex, uv + tsize * vec2(-1.0, -1.0)).x;
    float B = texture2D(tex, uv + tsize * vec2( 1.0, -1.0)).x;
    float C = texture2D(tex, uv + tsize * vec2(-1.0,  1.0)).x;
    float D = texture2D(tex, uv + tsize * vec2( 1.0,  1.0)).x;

    float c1 = (A + B + C + D) * 0.25;

    return c1;
}

float FetchDepth(sampler2D tex, const vec2 uv)
{
    return texture2D(tex, uv).x;
}

in highp vec2 vUV0;
void main()
{
    if (vUV0.x <= 0.5 && vUV0.y <= 0.5)
        FragColor.r = texture2D(ShadowMap0, vUV0 * 2.0).x;
    else if (vUV0.x > 0.5 && vUV0.y <= 0.5)
        FragColor.r = texture2D(ShadowMap1, (vUV0 - vec2(0.5, 0.0)) * 2.0).x;
    else if (vUV0.x <= 0.5 && vUV0.y > 0.5)
        FragColor.r = texture2D(ShadowMap2, (vUV0 - vec2(0.0, 0.5)) * 2.0).x;
    else if (vUV0.x > 0.5 && vUV0.y > 0.5)
        FragColor.r = texture2D(ShadowMap3, (vUV0 - vec2(0.5, 0.5)) * 2.0).x;
    else
        FragColor.r = 1.0;
}
