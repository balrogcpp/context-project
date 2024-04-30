// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform vec2 TexelSize;

// https://github.com/Experience-Monks/glsl-fast-gaussian-blur/blob/5dbb6e97aa43d4be9369bdd88e835f47023c5e2a/9.glsl
float Gauss9(sampler2D tex, const vec2 uv, const vec2 tsize)
{
    float A = texture2D(tex, uv).r;
    float B = texture2D(tex, uv + tsize * vec2(0.0, 1.3846153846)).r;
    float C = texture2D(tex, uv - tsize * vec2(0.0, 1.3846153846)).r;
    float D = texture2D(tex, uv + tsize * vec2(0.0, 3.2307692308)).r;
    float E = texture2D(tex, uv - tsize * vec2(0.0, 3.2307692308)).r;

    float c1 = A * 0.2270270270;
    float c2 = (B + C) * 0.3162162162;
    float c3 = (D + E) * 0.0702702703;

    return c1 + c2 + c3;
}

in highp vec2 vUV0;
void main()
{
    FragColor.r = Gauss9(RT, vUV0, TexelSize);
    FragColor.g = texture2D(RT, vUV0).g;
}
