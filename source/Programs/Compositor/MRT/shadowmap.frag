// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D ShadowMap0;
uniform sampler2D ShadowMap1;
uniform sampler2D ShadowMap2;
uniform sampler2D ShadowMap3;

float Box(sampler2D tex, const vec2 uv)
{
    vec2 tsize = 1.0 / vec2(textureSize(tex, 0));
    float c = 0.0;
    for (int x = -2; x < 1; x++)
    for (int y = -2; y < 1; y++) {
        c += texture2D(tex, vec2(uv + vec2(x, y) * tsize)).x / 9.0;
    }

    return c;
}

float Downscale13(sampler2D tex, const vec2 uv)
{
    vec2 tsize = 1.0 / vec2(textureSize(tex, 0));
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


in highp vec2 vUV0;
void main()
{
    if (vUV0.x >= 0.0 && vUV0.x <= 0.5 && vUV0.y >= 0.0  && vUV0.y <= 0.5)
        FragColor.r = Downscale13(ShadowMap0, vUV0 * 2.0);
    else if (vUV0.x <= 1.0 && vUV0.x > 0.5 && vUV0.y >= 0.0  && vUV0.y <= 0.5)
        FragColor.r = Downscale13(ShadowMap1, (vUV0 - vec2(0.5, 0.0)) * 2.0);
    else if (vUV0.x >= 0.0 && vUV0.x <= 0.5 && vUV0.y <= 1.0 && vUV0.y > 0.5)
        FragColor.r = Downscale13(ShadowMap2, (vUV0 - vec2(0.0, 0.5)) * 2.0);
    else if (vUV0.x <= 1.0 && vUV0.x > 0.5 && vUV0.y <= 1.0 && vUV0.y > 0.5)
        FragColor.r = Box(ShadowMap3, (vUV0 - vec2(0.5, 0.5)) * 2.0);
    else
        FragColor.r = 1.0;
}
