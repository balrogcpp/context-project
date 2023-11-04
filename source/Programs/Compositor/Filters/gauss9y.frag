// created by Andrey Vasiliev

#include "header.glsl"

uniform sampler2D RT;

// https://github.com/Experience-Monks/glsl-fast-gaussian-blur/blob/5dbb6e97aa43d4be9369bdd88e835f47023c5e2a/9.glsl
vec3 Gauss9(sampler2D tex, const vec2 uv)
{
    vec2 tsize = 1.0 / vec2(textureSize(tex, 0));
    vec3 A = texture2D(tex, uv).xyz;
    vec3 B = texture2D(tex, uv + tsize * vec2(0.0, 1.3846153846)).xyz;
    vec3 C = texture2D(tex, uv - tsize * vec2(0.0, 1.3846153846)).xyz;
    vec3 D = texture2D(tex, uv + tsize * vec2(0.0, 3.2307692308)).xyz;
    vec3 E = texture2D(tex, uv - tsize * vec2(0.0, 3.2307692308)).xyz;

    vec3 c1 = A * 0.2270270270 + (B + C) * 0.3162162162 + (D + E) * 0.0702702703;

    return c1;
}

in highp vec2 vUV0;
void main()
{
    FragColor.rgb = Gauss9(RT, vUV0);
}
