// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

float Gauss9H(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    float A = texture2D(tex, uv).x;
    float B = texture2D(tex, uv + tsize * vec2(1.3846153846, 0.0)).x;
    float C = texture2D(tex, uv - tsize * vec2(1.3846153846, 0.0)).x;
    float D = texture2D(tex, uv + tsize * vec2(3.2307692308, 0.0)).x;
    float E = texture2D(tex, uv - tsize * vec2(3.2307692308, 0.0)).x;

    float c1 = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return c1;
}

in vec2 vUV0;
void main()
{
    vec2 TexelSize0 = 1.0 / vec2(textureSize(RT, 0));
    FragColor.r = Gauss9H(RT, vUV0, TexelSize0);
}
