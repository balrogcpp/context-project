// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;
uniform vec2 TexelSize;

// https://github.com/Experience-Monks/glsl-fast-gaussian-blur/blob/5dbb6e97aa43d4be9369bdd88e835f47023c5e2a/5.glsl
vec3 Gauss5(const sampler2D tex, const vec2 uv, const vec2 tsize)
{
    vec2 off = tsize * vec2(1.3333333333333333, 0.0);
    vec3 A = textureLod(tex, uv, 0.0).rgb;
    vec3 B = textureLod(tex, uv + off, 0.0).rgb;
    vec3 C = textureLod(tex, uv - off, 0.0).rgb;

    vec3 c1 = A * 0.29411764705882354;
    vec3 c2 = (B + C) * 0.35294117647058826;

    return c1 + c2;
}

in highp vec2 vUV0;
void main()
{
    FragColor.rgb = Gauss5(RT, vUV0, TexelSize);
}
